@ echo off

cd openssl
if not exist libcrypto.a (
    perl Configure no-deprecated no-shared -static mingw64
    make
)
cd ..

cd .\curl\lib
if not exist libcurl.a mingw32-make -f Makefile.m32 CFG=-ssl OPENSSL_PATH=../../openssl libcurl.a
cd ..\..

if exist build rd /q /s build
if not exist build md build
cd build
cmake .. -G "MSYS Makefiles"
make install
cd ..

:: gcc getinfo.c -s -static -I .\curl\include -L .\curl\lib -lcurl -lwldap32 -lws2_32 -DCURL_STATICLIB
:: gcc server-conf.c -s -static -I .\openssl\include -L .\openssl -lssl -lcrypto -lcrypt32 -lws2_32
