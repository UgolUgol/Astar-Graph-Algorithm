CC=g++
CFLAGS=-std=c++11 -O2 -c -Wall

all:graph

clean:
	rm -rf graph *.o

graph: main.o graph.o
	$(CC) main.o graph.o -o Debug/graph
main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp
graph.o: graph.cpp
	$(CC) $(CFLAGS) graph.cpp
