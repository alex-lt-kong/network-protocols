OPTS = -O3 -Wall -pedantic -Wc++-compat -Wextra 

main: serializer.out deserializer.out

serializer.out: serializer.cpp
	g++ serializer.cpp -o serializer.out $(OPT) -lcapnp -lkj
deserializer.out: deserializer.cpp
	g++ deserializer.cpp -o deserializer.out $(OPT) -lcapnp -lkj


	