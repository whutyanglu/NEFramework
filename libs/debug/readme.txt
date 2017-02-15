编译命令
1、openssl:
perl Configure VC-WIN32 --debug no-shared zlib --with-zlib-include="D:\\opensource\\zlib-1.2.8.tar\\zlib-1.2.8" --with-zlib-lib=zlibstat.lib

nmake

nmake install

2、curl :
nmake /f Makefile.vc mode=static VC=14 WITH_SSL=static WITH_ZLIB=static DEBUG=no MACHINE=x86

3、zlib
sln

