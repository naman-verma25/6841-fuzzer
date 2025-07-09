// This is the main C file written by Naman Verma z5590213 on 7/7/2025
// THis function is used for reading file inputs, mutations, executions and logging
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_INPUT_SIZE 1024
#define MAX_MUTATED_SIZE 2048

const char *dictionary[] = {
	"<script>alert(1)</script>",
	"' OR 1=1 --",
	"../../../../etc/passwd",
	"%s,%s,%s,%s,%s",
	"javascript:alert(1)",
	"UNION SELECT * FROM users;"
};
const int dictSize = sizeof(dictionary) / sizeof(dictionary[0]);
char *last_strat = "";

// function prototypes
void mutate(char *input, char *output);
bool run_target(const char *target_path, const char * input, int crash_id);

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("WRONG USAGE\nYou must provide a target file\n");
		return 1;
	}

	const char *target_path = argv[1];
	DIR *d = opendir("corpus");
	if(!d) {
		perror("corpus dir");
		return 1;
	}

	struct dirent *entry;
    struct dirent *files[256];
    int file_count = 0;

    while((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        files[file_count++] = entry;
    }

    if (file_count == 0) {
        printf("No Input Files Detected. Create One and try again");
        return 1;
    }

    printf("Availables Files for Injection\n");
    for(int i = 0; i < file_count; i++) {
        printf("[%d] %s\n", i, files[i]->d_name);
    }

    printf("Enter your choice Between 0 and %d ", (file_count - 1));
    int choice;
    scanf("%d", &choice);

    if(choice >= file_count || choice < 0) {
        printf("Invalid Choice");
        return 1;
    }


	char original[MAX_INPUT_SIZE], mutated[MAX_MUTATED_SIZE];
	int crash_id = 0;

	srand(time(NULL));

	for (int i = 0; i < file_count; i++) {
		if (choice != -1 && i != choice) continue;

		char path[256];
		snprintf(path, sizeof(path), "corpus/%s", files[i]->d_name);

		FILE *fp = fopen(path, "r");
		if (!fp) {
			perror("Failed to open input file");
			continue;
		}
		size_t bytes_read = fread(original, 1, MAX_INPUT_SIZE - 1, fp);
		original[bytes_read] = '\0';
		fclose(fp);

		mutate(original, mutated);
		bool crashed = run_target(target_path, mutated, crash_id++);
		if (!crashed) {
			crash_id--;
		}

		FILE *summary = fopen("reports/summary_log.txt", "a");
		fprintf(summary, "Tried input from %s using strategy %s → Crash: %s\n",
		        files[i]->d_name, last_strat, crashed ? "YES" : "NO");
		fclose(summary);
	}

	printf("Fuzzing complete. Total crashes: %d\n", crash_id);
	closedir(d);
	return 0;
}

// different approaches
void bit_flip(char *data, size_t len) {
    int byte = rand() % len;
    int bit = rand() % 8;
    data[byte] ^= (1 << bit);
}

void byte_flip(char*data, size_t len) {
    int byte = rand() % len;
    data[byte] ^= 0xFF;
}

void arithmetic_mutation(char *data, size_t len) {
    int byte = rand() % len;
    int delta = (rand() % 35) - 17;
    int new = (unsigned char)data[byte] + delta;
    if (new < 0) {
        new = 0;
    } else if (new > 255) {
        new = 255;
    }
    data[byte] = (char)new; 
}

void dictionary_insertion(char *data, size_t *len, size_t maxLen) {
    const char *dict = dictionary[rand() % dictSize];
    int insertPos = rand() % (*len + 1);
    int dictLen = strlen(dict);
    if(*len + dictLen >= maxLen) {
        return;
    }
    memmove(data + insertPos + dictLen, data + insertPos, *len - insertPos);
    memcpy(data + insertPos, dict, dictLen);
    *len += dictLen;
}

void havoc(char * data, size_t *len, size_t maxLen) {
    int mutations = rand() % 5 + 1;
    for (int i = 0; i < mutations; i++) {
        int choice = rand() % 4;
        switch (choice) {
            case 0: bit_flip(data, *len); break;
            case 1: byte_flip(data, *len); break;
            case 2: arithmetic_mutation(data, *len); break;
            case 3: dictionary_insertion(data, len, maxLen); break;
        }
    }
}

void mutate(char *input, char *output) {
	size_t len = strlen(input);
    memcpy(output, input, len + 1);
    size_t mutated_len = len;

    int strategy = rand() % 5;
    if (strategy == 0) last_strat = "Bit Flip";
    if (strategy == 1) last_strat = "Byte Flip";
    if (strategy == 2) last_strat = "Arithmetic";
    if (strategy == 3) last_strat = "Dictionary";
    if (strategy == 4) last_strat = "Havoc";
    switch (strategy) {
        case 0: bit_flip(output, mutated_len); break;
        case 1: byte_flip(output, mutated_len); break;
        case 2: arithmetic_mutation(output, mutated_len); break;
        case 3: dictionary_insertion(output, &mutated_len, MAX_MUTATED_SIZE); break;
        case 4: havoc(output, &mutated_len, MAX_MUTATED_SIZE); break;
    }

    output[mutated_len] = '\0';
}

bool run_target(const char *target_path, const char *input, int crash_id) {
	FILE *tmp = fopen("temp_input.txt", "w");
	fprintf(tmp, "%s", input);
	fclose(tmp);
	
	pid_t pid = fork();
	if (pid == 0) {
		execl(target_path, target_path, "temp_input.txt", NULL);
		exit(1);
	} else {
		int status;
		waitpid(pid, &status, 0);

		if (WIFSIGNALED(status)) {
			char reportfile[256];
            snprintf(reportfile, sizeof(reportfile), "crashes/crash_report_%d.txt", crash_id);
            FILE *rf = fopen(reportfile, "w");
            int signal = WTERMSIG(status);
            fprintf(rf, "Exit Signal: %d (e.g., SIGSEGV = 11)\n", signal);
            fprintf(rf,
                "=== Crash Report #%d ===\n"
                "Target: %s\n"
                "Timestamp: %ld\n"
                "Mutation Strategy: %s\n"
                "Input Length: %lu\n"
                "Crash Input: %s\n",
                crash_id,
                target_path,
                time(NULL),
                last_strat,
                strlen(input),
                input
            );
            fclose(rf);
            return true;
		}
	}
    remove("temp_input.txt");
    return false;
}