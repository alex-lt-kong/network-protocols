CC = gcc
INCS = -I/usr/include/librdkafka
CFLAGS = -O2 -Wall -pedantic -Wextra -Wc++-compat
LDFLAGS = -lrdkafka
#ASAN = -fsanitize=address -g
UBSAN = -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-divide-by-zero -g

main: forwarder.out

forwarder.out: forwarder.c
	$(CC) forwarder.c -o forwarder.out $(INCS) $(LDFLAGS) $(CFLAGS) $(ASAN) $(UBSAN)

.PHONY:
clean:
	rm *.out
