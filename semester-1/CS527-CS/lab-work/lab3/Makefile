CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
LDFLAGS =
OBJS    = runner.o main.o logger.o frame.o bus.o receiver_driver.o sender_driver.o sender_application.o receiver_application.o device_store.o

all: lab3_uart

lab3_uart: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

runner.o: runner.c runner.h constants.h bus.h receiver.h sender_application.h logger.h
main.o: main.c runner.h
logger.o: logger.c logger.h
frame.o: frame.c frame.h
bus.o: bus.c bus.h frame.h logger.h constants.h receiver.h
receiver_driver.o: receiver_driver.c receiver.h frame.h bus.h logger.h constants.h device_store.h
sender_driver.o: sender_driver.c sender_driver.h frame.h logger.h constants.h bus.h
sender_application.o: sender_application.c sender_application.h sender_driver.h logger.h
receiver_application.o: receiver_application.c receiver.h
device_store.o: device_store.c device_store.h constants.h logger.h

clean:
	rm -f *.o lab3_uart

.PHONY: all clean
