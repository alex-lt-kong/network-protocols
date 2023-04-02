OPTS = -O2 -Wall -pedantic -Wextra
LIBS = -lzmq
CXX = g++

main: publisher.out subscriber.out

publisher.out: publisher.cpp
	$(CXX) publisher.cpp -o publisher.out $(LIBS) $(OPTS)
subscriber.out: subscriber.cpp
	$(CXX) subscriber.cpp -o subscriber.out $(LIBS) $(OPTS)

.PHONY: clean
clean:
	rm *.o *.out
	