@ cd .\curl\lib
@ if not exist libcurl.a mingw32-make -f Makefile.m32 libcurl.a
@ cd ..\..

@ if exist build rd /q /s build
@ if not exist build md build
@ cd build
@ cmake .. -G "MinGW Makefiles"
@ mingw32-make install
@ cd ..

:: gcc getinfo.c -s -static -I .\curl\include -L .\curl\lib -lcurl -lwldap32 -lws2_32 -DCURL_STATICLIB
