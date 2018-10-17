CC=g++
CFLAGS=-Wall -g -ggdb
SOURCES=example.cpp SimpleChess.cpp
OBJECTS=$(SOURCES:.cpp=.o)

main: example.cpp
	$(CC) $(CFLAGS) -c $(SOURCES)
	$(CC) $(OBJECTS) $(CFLAGS) -o chess
	rm -rf *.o

clean:
	rm -rf $(OBJECTS) chess
