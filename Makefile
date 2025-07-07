# Makefile - Builds the C fuzzer project

CC = gcc
CFLAGS = -Wall -g

all: fuzzer

fuzzer: fuzzer.o mutator.o executor.o
	$(CC) $(CFLAGS) -o fuzzer fuzzer.o mutator.o executor.o

clean:
	rm -f *.o fuzzer temp_input_* crashes/*

