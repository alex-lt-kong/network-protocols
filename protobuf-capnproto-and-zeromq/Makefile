OPTS = -O3 -Wall -pedantic -Wc++-compat -Wextra 
CC = g++
main: serialization-benchmark.out zmq-client-single.out zmq-server-single.out zmq-client-multiple.out zmq-server-multiple.out

serialization-benchmark.out: serialization-benchmark.cpp serialization.o protobuf.o pb_test.pb.o capnp.o
	$(CC) serialization-benchmark.cpp protobuf.o serialization.o pb_test.pb.o capnp.o -o serialization-benchmark.out $(OPT) -lcapnp -lkj -lprotobuf

zmq-client-single.out:zmq-client-single.cpp serialization.o capnp.o
	$(CC) zmq-client-single.cpp serialization.o capnp.o -o zmq-client-single.out -lpthread -lzmqpp -lzmq -lsodium -lcapnp -lkj $(OPT)
zmq-server-single.out:zmq-server-single.cpp serialization.o capnp.o
	$(CC) zmq-server-single.cpp  serialization.o capnp.o -o zmq-server-single.out -lpthread -lzmqpp -lzmq -lsodium -lcapnp -lkj $(OPT)

zmq-client-multiple.out:zmq-client-multiple.cpp serialization.o capnp.o
	$(CC) zmq-client-multiple.cpp serialization.o capnp.o -o zmq-client-multiple.out -lpthread -lzmqpp -lzmq -lsodium -lcapnp -lkj $(OPT)
zmq-server-multiple.out:zmq-server-multiple.cpp serialization.o capnp.o
	$(CC) zmq-server-multiple.cpp  serialization.o capnp.o -o zmq-server-multiple.out -lpthread -lzmqpp -lzmq -lsodium -lcapnp -lkj $(OPT)

serialization.o: serialization.cpp serialization.h
	$(CC) serialization.cpp -c $(OPT) -fPIE

protobuf.o: ./protobuf/protobuf.cpp
	$(CC) -c ./protobuf/protobuf.cpp $(OPT) -fPIE

pb_test.pb.o: ./protobuf/pb_test.pb.cc 
	$(CC) -c ./protobuf/pb_test.pb.cc $(OPT) -fPIE

capnp.o: ./capnp/capnp.cpp ./capnp/capnp.h 
	$(CC) -c ./capnp/capnp.cpp $(OPT) -fPIE


clean:
	rm *.o *.out
	