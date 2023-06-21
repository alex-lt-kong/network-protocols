CC = gcc
INCS = -I/usr/include/librdkafka
CFLAGS = -O2 -Wall -pedantic -Wextra -Wc++-compat
LDFLAGS = -lrdkafka


main: forwarder.out

forwarder.out: forwarder.c
	$(CC) forwarder.c -o forwarder.out $(INCS) $(LDFLAGS) $(CFLAGS)

.PHONY:
clean:
	rm *.o
	rm *.out