# Makefile - Builds the C fuzzer project

CC = gcc
CFLAGS = -Wall -O2
OBJ = fuzzer.o mutator.o monitor.o utils.o

all: fuzzer

fuzzer: $(OBJ)
	$(CC) $(CFLAGS) -o fuzzer $(OBJ)

fuzzer.o: fuzzer.c mutator.h monitor.h utils.h
	$(CC) $(CFLAGS) -c fuzzer.c

mutator.o: mutator.c mutator.h
	$(CC) $(CFLAGS) -c mutator.c

monitor.o: monitor.c monitor.h
	$(CC) $(CFLAGS) -c monitor.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f *.o fuzzer
