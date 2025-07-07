#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include "executor.h"

int run_target(const char *target, const char *input_path) {
    pid_t pid = fork();

    if(pid == 0) {
        execl(target, target, input_path, NULL);
        exit(1);
    }

    int status;
    waitpid(pid, &status, 0);

    if(WIFSIGNALED(status)) {
        return CRASH;
    }

    return NORMAL;
}