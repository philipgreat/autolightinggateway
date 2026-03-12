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

.PHONY: all clean install