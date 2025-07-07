// This is the main C file written by Naman Verma z5590213 on 7/7/2025
// THis function is used for reading file inputs, mutations, executions and logging
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <sys.wait.h>

#define MAX_INPUT 4096

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("WRONG USAGE\nYou must provide a target and an input file\n");
		return 1;
	}

	const char *target = argv[1];
	const char *input = argv[2];
	char buffer[MAX_INPUT];

	// reading the input file
	FILE *fp = fopen(input, "rb");
	if (!fp) {
		perror("Failed to open input file");
		return 1;
	}
	size_t len = fread(buffer, 1, MAX_INPUT, fp);
	fclose(fp);

	//performing the mutations 100 times to see if one of them crashes the program
	for (int i = 0; i < 100; i++) {
		char mutated[MAX_INPUT];
		memcpy(mutated, buffer, len);

		//mutating function call
		mutate_input(mutated, len);

		char fname[64];
		snprintf(fname, sizeof(fname), "temp_input_%d", i);
		FILE *mutf = fopen(fname, "wb");
		fwrite(mutated, 1, len, mutf);
		fclose(mutf);

		// run the file on the target
		int crsh = run_target(target, fname);

		if(crsh == 1) {
			printf("! Crash detected on input #%d\n", i);
			char crashname[64];
			snprintf(crashname, sizeof(crashname), "crashes/crash_%d", i);
			FILE *cf = fopen(crashname, "wb");
			fwrite(mutated, 1, len, cf);
			fclose(cf);
		}
		
		unlink(fname);
	}
	return 0;
}