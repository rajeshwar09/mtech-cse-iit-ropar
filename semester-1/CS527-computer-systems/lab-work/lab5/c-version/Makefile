# Makefile for CS527 Lab 4

CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2 -MMD -MP

OBJS    = app.o bus.o crc16.o devices.o frame.o logger.o main.o
DEPS    = $(OBJS:.o=.d)
BIN     = lab5
STATE_DIR = .lab5state

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(DEPS) $(BIN) $(STATE_DIR)

.PHONY: all clean

-include $(DEPS)
