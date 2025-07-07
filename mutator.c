// This file implements the mutator function
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "mutator.h"

void mutate_input(char *data, size_t len) {
	srand(time(NULL) ^ getpid());
	int num_mutations = rand() % 5 + 1;

	for (int i = 0; i < num_mutations; i++) {
		size_t ind = rand() % len;
		data[ind] ^= 1 << (rand() % 8);
	}
}