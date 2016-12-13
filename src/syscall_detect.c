#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 256

int syscall_open(char *path, long oflag)
{
    int fd = -1;
    #ifdef __i386__
    __asm__ (
             "mov $5, %%eax;" // Open syscall number
             "mov %1, %%ebx;" // Address of our string
             "mov %2, %%ecx;" // Open mode
             "mov $0, %%edx;" // No create mode
             "int $0x80;"     // Straight to ring0
             "mov %%eax, %0;" // Returned file descriptor
             :"=r" (fd)
             :"m" (path), "m" (oflag)
             :"eax", "ebx", "ecx", "edx"
             );
    #elif __amd64__
    __asm__ (
             "mov $2, %%rax;" // Open syscall number
             "mov %1, %%rdi;" // Address of our string
             "mov %2, %%rsi;" // Open mode
             "mov $0, %%rdx;" // No create mode
             "syscall;"       // Straight to ring0
             "mov %%eax, %0;" // Returned file descriptor
             :"=r" (fd)
             :"m" (path), "m" (oflag)
             :"rax", "rdi", "rsi", "rdx"
             );
    #endif
    return fd;
 }

size_t syscall_gets(char *buffer, size_t buffer_size, int fd)
{
    size_t i;
    for(i = 0; i < buffer_size-1; i++)
    {
        size_t nbytes;
        #ifdef __i386__
        __asm__ (
                 "mov $3, %%eax;" // Read syscall number
                 "mov %1, %%ebx;" // File descriptor
                 "mov %2, %%ecx;" // Address of our buffer
                 "mov $1, %%edx;" // Read 1 byte
                 "int $0x80;"     // Straight to ring0
                 "mov %%eax, %0;" // Returned read byte number 
                 :"=r" (nbytes)
                 :"m" (fd), "r" (&(buffer[i]))
                 :"eax", "ebx", "ecx", "edx"
                 );
        #elif __amd64__
        __asm__ (
                 "mov $0, %%rax;" // Read syscall number
                 "mov %1, %%rdi;" // File descriptor
                 "mov %2, %%rsi;" // Address of our buffer
                 "mov $1, %%rdx;" // Read 1 byte
                 "syscall;"       // Straight to ring0
                 "mov %%rax, %0;" // Returned read byte number
                 :"=r" (nbytes)
                 :"m" (fd), "r" (&(buffer[i]))
                 :"rax", "rdi", "rsi", "rdx"
                 );
        #endif
        if(nbytes != 1)
            break;
        if(buffer[i] == '\n')
        {
            i++;
            break;
        }
    }
    buffer[i] = '\0';
    return i;
}

// Avoid to use libc strstr
char* afterSubstr(char *str, const char *sub)
{
    int i, found;
    char *ptr;
    found = 0;
    for(ptr = str; *ptr != '\0'; ptr++)
    {
        found = 1;
        for(i = 0; found == 1 && sub[i] != '\0'; i++)
            if(sub[i] != ptr[i])
                found = 0;
        if(found == 1)
            break;
    }
    if(found == 0)
        return NULL;
    return ptr + i;
}

// Try to match the following regexp: libname-[0-9]+\.[0-9]+\.so$
// Not using any libc function makes that code awful, I know
int isLib(char *str, const char *lib)
{
    int i, found;
    static const char *end = ".so\n";
    char *ptr;
    // Trying to find lib in str
    ptr = afterSubstr(str, lib);
    if(ptr == NULL)
        return 0;
    // Should be followed by a '-'
    if(*ptr != '-')
        return 0;
    // Checking the first [0-9]+\.
    found = 0;
    for(ptr += 1; *ptr >= '0' && *ptr <= '9'; ptr++)
        found = 1;
    if(found == 0 || *ptr != '.')
        return 0;
    // Checking the second [0-9]+
    found = 0;
    for(ptr += 1; *ptr >= '0' && *ptr <= '9'; ptr++)
        found = 1;
    if(found == 0)
        return 0;
    // Checking if it ends with ".so\n"
    for(i = 0; end[i] != '\0'; i++)
        if(end[i] != ptr[i])
            return 0;
    return 1;
}

int main()
{
    int memory_map;
    char buffer[BUFFER_SIZE];
    int after_libc = 0;

    // If the file was succesfully opened
    if(syscall_open("/etc/ld.so.preload", O_RDONLY) > 0)
        printf("/etc/ld.so.preload detected through open syscall\n");
    else
        printf("/etc/ld.so.preload is not present\n");
    // Open the memory map through a syscall this time
    memory_map = syscall_open("/proc/self/maps", O_RDONLY);
    if(memory_map == -1)
    {
        printf("/proc/self/maps is unaccessible, probably a LD_PRELOAD attempt\n");
        return 1;
    }
    // Read the memory map line by line
    // Try to look for a library loaded in between the libc and ld
    while(syscall_gets(buffer, BUFFER_SIZE, memory_map) != 0)
    {
        // Look for a libc entry
        if(isLib(buffer, "libc"))
            after_libc = 1;
        else if(after_libc)
        {
            // Look for a ld entry
            if(isLib(buffer, "ld"))
            {
                // If we got this far then everythin is fine
                printf("Memory maps are clean\n");
                break;
            }
            // If it's not an anonymous memory map
            else if(afterSubstr(buffer, "00000000 00:00 0") == NULL)
            {
                // Something has been preloaded by ld.so
                printf("LD_PRELOAD detected through memory maps\n");
                break;
            }
        }
    }
}
