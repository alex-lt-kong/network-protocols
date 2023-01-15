OPTS = -O3 -Wall -pedantic -Wc++-compat -Wextra 
CC = clang++
main: serialization-benchmark.out zmq-server.out zmq-client.out

serialization-benchmark.out: serialization-benchmark.cpp serialization.o protobuf.o person.pb.o capnp.o
	$(CC) serialization-benchmark.cpp protobuf.o serialization.o person.pb.o capnp.o -o serialization-benchmark.out $(OPT) -lcapnp -lkj -lprotobuf

zmq-client.out:zmq-client.cpp serialization.o capnp.o
	$(CC) zmq-client.cpp serialization.o capnp.o -o zmq-client.out -lpthread -lzmqpp -lzmq -lsodium -lcapnp -lkj $(OPT)

zmq-server.out:zmq-server.cpp serialization.o capnp.o
	$(CC) zmq-server.cpp  serialization.o capnp.o -o zmq-server.out -lpthread -lzmqpp -lzmq -lsodium -lcapnp -lkj $(OPT)

serialization.o: serialization.cpp serialization.h
	$(CC) serialization.cpp -c $(OPT)

protobuf.o: ./protobuf/protobuf.cpp
	$(CC) -c ./protobuf/protobuf.cpp $(OPT)

person.pb.o: ./protobuf/person.pb.cc 
	$(CC) -c ./protobuf/person.pb.cc $(OPT)

capnp.o: ./capnp/capnp.cpp
	$(CC) -c ./capnp/capnp.cpp $(OPT)


clean:
	rm *.o *.out
	