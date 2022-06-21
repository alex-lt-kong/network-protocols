main: tcp-server.o tcp-client.o tcp-client-multiple-sends.o udp-server.o udp-client.o tcp-client-check-order.o tcp-server-check-order.o
	gcc tcp-server.o -o tcp-server.out
	gcc tcp-client.o -o tcp-client.out
	gcc tcp-client-multiple-sends.o -o tcp-client-multiple-sends.out
	gcc udp-server.o -o udp-server.out
	gcc udp-client.o -o udp-client.out
	gcc tcp-client-check-order.o -o tcp-client-check-order.out
	gcc tcp-server-check-order.o -o tcp-server-check-order.out

tcp-client-check-order.o: tcp-client-check-order.c
	gcc -c tcp-client-check-order.c

tcp-server-check-order.o: tcp-server-check-order.c
	gcc -c tcp-server-check-order.c

tcp-server.o: tcp-server.c
	gcc -c tcp-server.c

tcp-client.o: tcp-client.c
	gcc -c tcp-client.c

tcp-client-multiple-sends.o: tcp-client-multiple-sends.c
	gcc -c tcp-client-multiple-sends.c

udp-server.o: udp-server.c
	gcc -c udp-server.c

udp-client.o: udp-client.c
	gcc -c udp-client.c

clean:
	rm *.out
	rm *.o