CC = gcc
CFLAGS = -Wall -Werror -I./include
TARGET = lights
OBJS = main.o serialib.o udp_server.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

install:
	cp $(TARGET) /usr/local/bin/


sync:
	rsync -avz --exclude .git --exclude lights --exclude lights-armv7 ./* root@192.168.2.1:/data/workspace/auto-light
	rsync -avz --exclude .git --exclude lights --exclude lights-armv7 ./* root@192.168.1.1:/data/workspace/auto-light
	
.PHONY: all clean install