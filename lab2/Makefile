CC ?= gcc
CFLAGS = -std=gnu11 -g -I./include

TARGET = server client

SERVER_OBJS = src/server.o src/utils.o
CLIENT_OBJS = src/client.o src/utils.o

all: $(TARGET)

include common.mk

%.o: %.c
	$(VECHO) "   CC\t$@\n"
	$(Q)$(CC) -c $< $(CFLAGS) -o $@

server: $(SERVER_OBJS)
	$(VECHO) "   LD\t$@\n"
	$(Q)$(CC) -o $@ $^ $(CFLAGS)

client: $(CLIENT_OBJS)
	$(VECHO) "   LD\t$@\n"
	$(Q)$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(TARGET) $(SERVER_OBJS) $(CLIENT_OBJS)
.PHONY: clean
