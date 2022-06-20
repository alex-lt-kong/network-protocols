main: server.o client.o
	gcc server.o -o server.out
	gcc client.o -o client.out

server.o: server.c
	gcc -c server.c

client.o: client.c
	gcc -c client.c

clean:
	rm *.out
	rm *.o