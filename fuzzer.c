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
    "../../../../../../etc/passwd",
    "%x %x %x %x",
    "%n%n%n",
    "$(reboot)",
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
    "AAAA.%s.%x.%n.BBBB",
    "DROP TABLE users;",
    "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
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
	char *filenames[256];
	int file_count = 0;

	while ((entry = readdir(d)) != NULL) {
		if (entry->d_name[0] == '.') {
			continue;
		}
		filenames[file_count++] = strdup(entry->d_name);
	}

    if (file_count == 0) {
        printf("No Input Files Detected. Create One and try again");
        return 1;
    }

	char original[MAX_INPUT_SIZE], mutated[MAX_MUTATED_SIZE];
	int crash_id = 0;

	srand(time(NULL));

	for (int i = 0; i < file_count; i++) {

		char path[256];
		snprintf(path, sizeof(path), "corpus/%s", filenames[i]);

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
		        filenames[i], last_strat, crashed ? "YES" : "NO");
		fclose(summary);
	}

    for (int i = 0; i < file_count; i++) {
        free(filenames[i]);
    }

	printf("Fuzzing complete. Total crashes: %d\n", crash_id);
    for(int i = 0; i < crash_id; i++) {
        printf("[%d] View the crash report at crashes/crash_report_%d.txt\n", i, i);
    }
    printf("And View the Summary at reports/summary_log.txt\n");
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

void repeat(char *data, size_t *len, size_t maxLen) {
    const char *pat = "AAAA";
    int reps = rand() % 100 + 50;
    int total = reps * strlen(pat);

    if (*len + total >= maxLen) {
        return;
    }

    for (int i = 0; i < reps && *len + 4 < maxLen; i++) {
    memcpy(data + *len, pat, 4);
    *len += 4;
    }
    data[*len] = '\0';
}

void havoc(char * data, size_t *len, size_t maxLen) {
    int mutations = rand() % 5 + 1;
    for (int i = 0; i < mutations; i++) {
        int choice = rand() % 5;
        switch (choice) {
            case 0: bit_flip(data, *len); break;
            case 1: byte_flip(data, *len); break;
            case 2: arithmetic_mutation(data, *len); break;
            case 3: dictionary_insertion(data, len, maxLen); break;
            case 4: repeat(data, len, maxLen); break;
        }
    }
}

void mutate(char *input, char *output) {
	size_t len = strlen(input);
    memcpy(output, input, len + 1);
    size_t mutated_len = len;

    int strategy = rand() % 6;
    if (strategy == 0) last_strat = "Bit Flip";
    if (strategy == 1) last_strat = "Byte Flip";
    if (strategy == 2) last_strat = "Arithmetic";
    if (strategy == 3) last_strat = "Dictionary";
    if (strategy == 4) last_strat = "Repeat";
    if (strategy == 5) last_strat = "Havoc";
    switch (strategy) {
        case 0: bit_flip(output, mutated_len); break;
        case 1: byte_flip(output, mutated_len); break;
        case 2: arithmetic_mutation(output, mutated_len); break;
        case 3: dictionary_insertion(output, &mutated_len, MAX_MUTATED_SIZE); break;
        case 4: repeat(output, &mutated_len, MAX_MUTATED_SIZE); break;
        case 5: havoc(output, &mutated_len, MAX_MUTATED_SIZE); break;
    }

    output[mutated_len] = '\0';
}

#include <fcntl.h>

// Updated run_target
bool run_target(const char *target_path, const char *input, int crash_id) {
    FILE *tmp = fopen("temp_input.txt", "w");
    fprintf(tmp, "%s", input);
    fclose(tmp);

    char stderr_file[256];
    snprintf(stderr_file, sizeof(stderr_file), "errors/stderr_%d.txt", crash_id);

    pid_t pid = fork();
    if (pid == 0) {
        // CHILD
        int fd = open(stderr_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fd, STDERR_FILENO); // Redirect stderr to file
        close(fd);

        execl(target_path, target_path, "temp_input.txt", NULL);
        exit(100); // If execl fails
    } else {
        // PARENT
        int status;
        waitpid(pid, &status, 0);

        bool crashed = false;
        int signal = -1;

        if (WIFSIGNALED(status)) {
            signal = WTERMSIG(status);
            crashed = true;
        } else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            crashed = true;
        }

        if (crashed) {
            char reportfile[256];
            snprintf(reportfile, sizeof(reportfile), "crashes/crash_report_%d.txt", crash_id);
            FILE *rf = fopen(reportfile, "w");
            if (!rf) {
                perror("Failed to create crash report");
                return true;
            }

            // Write basic info
            fprintf(rf,
                "====================[ Crash Report #%d ]====================\n"
                "🧪 Target:            %s\n"
                "📅 Timestamp:         %ld\n"
                "🧬 Mutation Strategy: %s\n"
                "📏 Input Length:      %lu bytes\n"
                "🚨 Exit Signal:       %d\n"
                "------------------------------------------------------------\n"
                "📝 Crash Input:\n%s\n"
                "------------------------------------------------------------\n",
                crash_id,
                target_path,
                time(NULL),
                last_strat,
                strlen(input),
                signal,
                input
            );

            // Append ASan stderr output
            FILE *errf = fopen(stderr_file, "r");
            if (errf) {
                fprintf(rf, "\n--- ASan Output ---\n");
                char line[1024];
                while (fgets(line, sizeof(line), errf)) {
                    fputs(line, rf);
                }
                fclose(errf);
            } else {
                fprintf(rf, "\n(No ASan stderr output captured)\n");
            }

            fclose(rf);
            return true;
        }
    }

    remove("temp_input.txt");
    return false;
}

