@ cd openssl
@ perl Configure no-deprecated no-shared mingw64
@ make install_sw
@ cd ..

:: gcc server-conf.c -s -static -I .\openssl\include -L .\openssl -lssl -lcrypto -lcrypt32 -lws2_32
