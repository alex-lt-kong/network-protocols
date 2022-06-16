INCS = -I/usr/include/librdkafka
LIBS = -lrdkafka

main: forwarder.o
	gcc forwarder.o -o forwarder.out $(INCS) $(LIBS)

forwarder.o: forwarder.c
	gcc -c forwarder.c $(INCS) $(LIBS)

clean:
	rm *.o
	rm *.out