#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define FILE_SEPARATOR "\x55\xAA\x5A\xA5"

int main(int argc, char* argv[])
{
    FILE *input = ((argc == 2 && strcmp(argv[1], "-") != 0) ? fopen(argv[1], "rb") : stdin);
    uint8_t word[4];
    while (fread(&word, sizeof(word), 1, input)) {
        if (memcmp(word, FILE_SEPARATOR, sizeof(word)) == 0) {
            printf("found file separator\n");
        } else {
            //printf("read instead %08x\n", word);
        }
    }
    fclose(input);
    return 0;
}

