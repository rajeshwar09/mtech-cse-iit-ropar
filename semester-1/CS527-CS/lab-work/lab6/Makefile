CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -O2

SRC := \
  devices.c \
  bus.c \
  driver.c \
  app.c \
  main.c

HDR := \
  constants.h \
  crc16.h \
  frame.h \
  devices.h \
  bus.h \
  driver.h \
  app.h

OBJ := $(SRC:.c=.o)
BIN := lab7_c_device_only

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean