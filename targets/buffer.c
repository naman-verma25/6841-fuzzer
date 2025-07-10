#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    FILE *f = fopen(argv[1], "r");
    if (!f) return 2;

    char buf[16];
    fread(buf, 1, 512, f);  // Overreads buffer
    fclose(f);

    printf("Received: %s\n", buf);
    return 0;
}
