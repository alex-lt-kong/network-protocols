OPTS = -O3 -Wall -pedantic -Wc++-compat -Wextra 

main: serialization.out

serialization.out: serialization.cpp person.pb.cc
	g++ serialization.cpp person.pb.cc -o serialization.out $(OPT) -lprotobuf


clean:
	rm *.out