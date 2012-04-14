#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crc.h"

#define TEMP_FILE_FOR_CRC_CHECK "/tmp/crc_temp_checking"

crc_t *
crc16(const char *data, int length)
{
    FILE *pipe;
    FILE *tmp;
    crc_t *result;
    char byte;
    int i;

    result = calloc(sizeof(crc_t), 1);
    result->crc = malloc(length / 1024 + 2);
    remove(TEMP_FILE_FOR_CRC_CHECK);
    tmp = fopen(TEMP_FILE_FOR_CRC_CHECK, "wb+");
    fwrite(data, sizeof(char), length, tmp);
    fclose(tmp);
    pipe = popen("./crc " TEMP_FILE_FOR_CRC_CHECK, "r");
    for(i = 0; !feof(pipe); i++) {
        fscanf(pipe, "%X", result->crc + i);
    }
    result->crc[i] = '\0';
    result->length = i;
    fclose(pipe);

    return result;
}

crc_t *
new_crc_t(const char *crc, const size_t length)
{
    crc_t *result;

    result = malloc(sizeof(crc_t));
    result->crc = crc;
    result->length = length;

    return result;
}
