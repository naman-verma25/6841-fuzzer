#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f) return 2;

    char buf[32];
    fread(buf, 1, sizeof(buf) - 1, f);
    buf[31] = '\0';
    fclose(f);

    char crash[16];
    strcpy(crash, buf);
    return 0;
}
