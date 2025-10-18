CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
OBJS    = main.o app.o bus.o devices.o frame.o crc16.o

all: lab5

lab5: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: main.c app.h constants.h
app.o: app.c app.h bus.h frame.h constants.h
bus.o: bus.c bus.h devices.h frame.h constants.h
devices.o: devices.c devices.h constants.h
frame.o: frame.c frame.h constants.h crc16.h
crc16.o: crc16.c crc16.h

clean:
	rm -f *.o lab5

.PHONY: all clean
