#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    FILE *f = fopen(argv[1], "r");
    if (!f) return 2;

    char input[256];
    fread(input, 1, 255, f);
    fclose(f);
    input[255] = '\0';

    printf(input);
    return 0;
}
