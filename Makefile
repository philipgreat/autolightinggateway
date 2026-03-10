TOOL_HOME=/home/philip/openwrt/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2

#CROSS_COMPILE = mips-openwrt-linux-
CROSS_COMPILE = arm-linux-gnueabi-
ARCH = armv7

CFLAGS= -Wall -Werror -I $(TOOL_HOME)/include -I ./include
LDFLAGS=-L$(TOOL_HOME)/lib -L ./lib
LIBS=-lc  -lpthread -ldl -lrt
CC = $(CROSS_COMPILE)gcc  $(CFLAGS)
LD = $(CROSS_COMPILE)gcc  $(LDFLAGS) $(LIBS) -static
MAKE = make

OBJS = main.o serialib.o udp_server.o

all: lights
	cp -f lights ./lights-$(ARCH)
	cp ./lights-$(ARCH)  ~/website/kg2x-com-website/

lights: $(OBJS)
	$(LD) -o lights $(OBJS)
main.o: main.c
	$(CC) -c main.c
serialib.o: serialib.c serialib.h
	$(CC) -c serialib.c
udp_server.o: udp_server.c udp_server.h
	$(CC) -c udp_server.c
clean:
	rm -f *.o

