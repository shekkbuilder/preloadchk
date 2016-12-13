#define _GNU_SOURCE
//check for false postitives
//libcfile=$(strace -e trace=open ldd --version 2>&1|grep -Po '(?<=").*libc.*\.so\.6(?=")')
//objdump -tT ${libcfile} | grep -P '\bfopen\b(?!@)'|awk '{print $1}'|xargs|while read addy1 addy2;do echo $((0x$addy2 - 0x$addy1));done

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if(argc != 2)
  {
    printf("%s <libc.so.6 location>\n", argv[0]);
    exit(-1);
  }
  size_t len = strlen(argv[1]);
  char * LIBC = malloc(len+1);
  if(LIBC==NULL)
  {
    return 1;
  }
  strncpy(LIBC, argv[1], len);
  void *libc = dlopen(LIBC, RTLD_LAZY); // Open libc directly with dlopen
  printf("LIBC: %s\n", LIBC);
  char *syscalls[] = {"open", "readdir", "fopen", "accept", "access", "unlink"};
  int i;
  void *(*libc_func)();
  void *(*next_func)();

  for (i = 0; i < 6; ++i)
  {
    printf("[+] Checking %s syscall.\n", syscalls[i]);
    libc_func = dlsym(libc, syscalls[i]);
    next_func = dlsym(RTLD_NEXT, syscalls[i]);
    if (libc_func != next_func)
    {
      printf("[!] Preload hooks detected!\n");
      printf("Libc address: %p\n", libc_func);
      printf("Next address: %p\n", next_func);
    }
    //free(LIBC);
  }

  return 0;
}
