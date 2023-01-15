OPTS = -O3 -Wall -pedantic -Wc++-compat -Wextra 
CC = clang++
main: serialization.out zmq-server.out zmq-client.out

serialization.out: serialization.cpp protobuf.o person.pb.o capnp.o
	$(CC) serialization.cpp protobuf.o person.pb.o capnp.o -o serialization.out $(OPT) -lcapnp -lkj -lprotobuf

protobuf.o: ./protobuf/protobuf.cpp
	$(CC) -c ./protobuf/protobuf.cpp $(OPT)

person.pb.o: ./protobuf/person.pb.cc 
	$(CC) -c ./protobuf/person.pb.cc $(OPT)

capnp.o: ./capnp/capnp.cpp
	$(CC) -c ./capnp/capnp.cpp $(OPT)

zmq-client.out:zmq-client.cpp
	$(CC) zmq-client.cpp -o zmq-client.out -lpthread -lzmqpp -lzmq -lsodium  $(OPT)

zmq-server.out:zmq-server.cpp
	$(CC) zmq-server.cpp -o zmq-server.out -lpthread -lzmqpp -lzmq -lsodium  $(OPT)

clean:
	rm *.o *.out
	