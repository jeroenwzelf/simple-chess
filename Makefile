CC=g++
CFLAGS=-Wall -g -ggdb
SOURCES=SimpleChess.cpp
OBJECTS=$(SOURCES:.cpp=.o)

main: SimpleChess.cpp
	$(CC) $(CFLAGS) -c $(SOURCES)
	$(CC) $(OBJECTS) $(CFLAGS) -o chess
	rm -rf *.o

clean:
	rm -rf $(OBJECTS) chess
