main: server.o client.o client-msgmore.o
	gcc server.o -o server.out
	gcc client.o -o client.out
	gcc client-msgmore.o -o client-msgmore.out

server.o: server.c
	gcc -c server.c

client.o: client.c
	gcc -c client.c

client-msgmore.o: client-msgmore.c
	gcc -c client-msgmore.c

clean:
	rm *.out
	rm *.o