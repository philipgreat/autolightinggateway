TOOL_HOME=/home/philip/openwrt/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2

CROSS_COMPILE = mips-openwrt-linux-
ARCH = mips

CFLAGS= -Wall -Werror -I $(TOOL_HOME)/include -I ./include
LDFLAGS=-L$(TOOL_HOME)/lib -L ./lib
LIBS=-lc  -lpthread -ldl -lrt
CC = $(CROSS_COMPILE)gcc  $(CFLAGS)
LD = $(CROSS_COMPILE)gcc  $(LDFLAGS) $(LIBS)
MAKE = make

OBJS = main.o serialib.o udp_server.o

all: lights
	cp lights /usr/share/nginx/html/

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

