Original src code and concepts come from chokepoint.net and haxelion.eu:
http://www.chokepoint.net/2014/02/detecting-userland-preload-rootkits.html
http://haxelion.eu/article/LD_NOT_PRELOADED_FOR_REAL/

LD_PRELOAD detection:
http://fluxius.handgrep.se/2011/10/31/the-magic-of-ld_preload-for-userland-rootkits/
https://ellzey.github.io/2014/03/22/preload-reload/
http://security.stackexchange.com/questions/56207/linux-kernel-3-system-call-table-modification-detection
https://github.com/milabs/kmod_hooking
https://github.com/CodeAndData/kerokid
https://github.com/dgoulet/kjackal

HOOKING:
http://www.gilgalab.com.br/hacking/programming/linux/2013/01/11/Hooking-Linux-3-syscalls/
http://hackerboss.com/overriding-system-functions-for-fun-and-profit/
https://tnichols.org/2015/10/19/Hooking-the-Linux-System-Call-Table/
http://samanbarghi.com/blog/2014/09/05/how-to-wrap-a-system-call-libc-function-in-linux/
http://syprog.blogspot.com/2011/10/hijack-linux-system-calls-part-iii.html
https://ruinedsec.wordpress.com/2013/04/04/modifying-system-calls-dispatching-linux/
http://opensourceforu.com/2011/08/lets-hook-a-library-function/
https://www.reddit.com/r/ReverseEngineering/comments/1773m2/system_call_interposition_in_userspace_without_ld/
https://rafalcieslak.wordpress.com/2013/04/02/dynamic-linker-tricks-using-ld_preload-to-cheat-inject-features-and-investigate-programs/
https://blog.netspi.com/function-hooking-part-i-hooking-shared-library-function-calls-in-linux/
http://rpinfosec.blogspot.com/2012/06/hooking-linux-system-call-table-in-2012.html
http://adhokshajmishraonline.in/2015/03/hooking-system-call-in-linux/
http://shadowwhowalks.blogspot.com/2013/01/android-hacking-hooking-system.html
http://turbochaos.blogspot.com/2013/10/writing-linux-rootkits-201-23.html
https://exploit.ph/linux-kernel-hacking/2014/07/10/system-call-hooking/
http://phrack.org/issues/68/6.html
http://onestraw.net/linux/lkm-and-syscall-hook/
https://memset.wordpress.com/2010/12/03/syscall-hijacking-kernel-2-6-systems/
https://exploit.ph/linux-kernel-hacking/2014/07/10/system-call-hooking/
http://se7so.blogspot.com/2012/07/hijacking-linux-system-calls-rootkit.html
http://www.compsoc.man.ac.uk/~moz/kernelnewbies/code/intercept/index.php3
http://stackoverflow.com/questions/13876369/system-call-interception-in-linux-kernel-module-kernel-3-5
http://okmij.org/ftp/syscall-interpose.html
https://fakeroot-ng.lingnu.com/index.php/PTRACE_LD_PRELOAD_comparison
https://www.greysec.net/showthread.php?tid=438
https://blog.gopheracademy.com/advent-2015/libc-hooking-go-shared-libraries/

EXAMPLES:
https://github.com/shekkbuilder/sysread_interceptor
https://poppopret.org/2013/01/07/suterusu-rootkit-inline-kernel-function-hooking-on-x86-and-arm/
https://github.com/ebradbury/linux-syscall-hooker
http://www.immunityinc.com/downloads/linux_rootkit_source.tbz2
https://github.com/kevinkoo001/rootkit
https://github.com/schischi/slrk
https://github.com/NoviceLive/research-rootkit
http://shell-storm.org/blog/Simple-Hook-detection-Linux-module/
http://dandylife.net/blog/archives/304
https://github.com/sancao2/reading-notes/blob/master/linux-rootkit-hack.org
https://github.com/shekkbuilder/sysread_interceptor
https://github.com/bawejakunal/Intercept-System-Calls
https://raw.githubusercontent.com/mempodippy/vlany/master/README
https://github.com/chokepoint/Jynx2
