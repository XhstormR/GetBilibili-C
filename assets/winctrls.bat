@ cd windows
@ busybox awk "NR<1165 {print > \"1234.c\"}" winctrls.c
@ cproto -I.././ -I../charset/ -I../windows/ -I../unix/ -D_WINDOWS -o1234.h 1234.c
@ gcc -I.././ -I../charset/ -I../windows/ -I../unix/ -D_WINDOWS -E -o1234.i 1234.c
@ cd ..

:: https://git.tartarus.org/?p=simon/putty.git;a=blob_plain;f=windows/winctrls.c;hb=HEAD
