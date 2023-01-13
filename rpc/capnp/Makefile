OPTS = -O3 -Wall -pedantic -Wc++-compat -Wextra 

main: serialization.out

serialization.out: serialization.cpp
	g++ serialization.cpp -o serialization.out $(OPT) -lcapnp -lkj


clean:
	rm *.out