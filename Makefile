CCX64	:= x86_64-w64-mingw32-gcc

CFLAGS	:= -Os -fno-asynchronous-unwind-tables
CFLAGS 	+= -fno-ident -fpack-struct=8 -falign-functions=1
CFLAGS  += -s -ffunction-sections -falign-jumps=1 -w
CFLAGS	+= -falign-labels=1 -fPIC
CFLAGS	+= -Wl,-s,--no-seh,--enable-stdcall-fixup

OUTX64	:= bin/GhostInject.x64.exe

all: x64

x64:
	@ echo Compile executable...
	@ $(CCX64) src/*.c  -o $(OUTX64) $(CFLAGS) $(LFLAGS) -Iinclude -masm=intel

clean:
	@ rm -rf bin/*.o
	@ rm -rf bin/*.bin
	@ rm -rf bin/*.exe