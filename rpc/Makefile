OPTS = -O3 -Wall -pedantic -Wc++-compat -Wextra 

main: serialization.out

serialization.out: serialization.cpp ./protobuf/person.pb.cc
	g++ serialization.cpp ./protobuf/person.pb.cc -o serialization.out $(OPT) -lcapnp -lkj -lprotobuf
 

clean:
	rm *.out