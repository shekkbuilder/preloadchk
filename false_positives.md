[root@nop .sec]# libcfile=$(strace -e trace=open ldd --version 2>&1|grep -Po '(?<=").*libc.*\.so\.6(?=")')
[root@nop .sec]# ./preloadchk ${libcfile}
LIBC: /lib/libc.so.6
[+] Checking open syscall.
[+] Checking readdir syscall.
[+] Checking fopen syscall.
[!] Preload hooks detected!
Libc address: 0xb7669160
Next address: 0xb772d470
[+] Checking accept syscall.
[+] Checking access syscall.
[+] Checking unlink syscall.

[root@nop .sec]# objdump -tT ${libcfile} | grep -P '\bfopen\b(?!@)'
00061160 g    DF .text  00000032  GLIBC_2.1   fopen
00125470 g    DF .text  00000088 (GLIBC_2.0)  fopen

[root@nop .sec]# objdump -tT ${libcfile} | grep -P '\bfopen\b(?!@)'|awk '{print $1}'|xargs|while read addy1 addy2;do echo $((0x$addy$
803600

[root@nop .sec]# echo $((0xb772d470 - 0xb7669160))
803600
