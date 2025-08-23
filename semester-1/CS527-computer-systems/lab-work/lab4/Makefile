# Makefile for CS527 Lab 4

CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2 -MMD -MP

OBJS    = logger.o main.o crc16.o frame.o receiver_driver.o receiver_application.o bus.o sender_driver.o sender_application.o
DEPS    = $(OBJS:.o=.d)
BIN     = lab4
STATE_DIR = .lab4state

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Generic compile rule (auto-generates .d dependency files)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(DEPS) $(BIN) $(STATE_DIR)

.PHONY: all clean

# Rebuild when headers change
-include $(DEPS)
