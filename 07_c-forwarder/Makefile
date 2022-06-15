INCS = -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/librdkafka
LIBS = -lglib-2.0  -lrdkafka -lrdkafka++ -lbsd

main: forwarder.o
	gcc forwarder.o -o forwarder.out $(INCS) $(LIBS)

forwarder.o: forwarder.c
	gcc -c forwarder.c $(INCS) $(LIBS)

common.o: common.c
	gcc -c common.c $(INCS) $(LIBS)

clean:
	rm *.o
	rm *.out