OPTS = -O3 -Wall -pedantic -Wc++-compat -Wextra 

main: serialization.out

serialization.out: serialization.cpp protobuf.o person.pb.o
	g++ serialization.cpp protobuf.o person.pb.o -o serialization.out $(OPT) -lcapnp -lkj -lprotobuf

protobuf.o: ./protobuf/protobuf.cpp
	g++ -c ./protobuf/protobuf.cpp $(OPT) -I./protobuf

person.pb.o: ./protobuf/person.pb.cc 
	g++ -c ./protobuf/person.pb.cc $(OPT) -I./protobuf

clean:
	rm *.o *.out
	