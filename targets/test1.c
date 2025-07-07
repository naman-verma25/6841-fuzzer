#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) return 1;

    char buf[100];
    fread(buf, 1, sizeof(buf), fp);
    fclose(fp);

    if (strstr(buf, "CRASHME")) {
        printf("Oops!\n");
        *(int *)0 = 0;
    }

    return 0;
}
