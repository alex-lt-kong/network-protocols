OPTS = -O3 -Wall -pedantic -Wc++-compat -Wextra 

main: serializer.out test.out

serializer.out: serializer.cpp
	g++ serializer.cpp -o serializer.out $(OPT) -lcapnp -lkj

test.out: test.cpp
	g++ test.cpp -o test.out $(OPT) -lcapnp -lkj

clean:
	rm *.out