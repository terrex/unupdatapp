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
    //char *crc;
    char crc[1000000];
    crc_t *result;
    int i;

    result = calloc(sizeof(crc_t), 1);
    remove(TEMP_FILE_FOR_CRC_CHECK);
    tmp = fopen(TEMP_FILE_FOR_CRC_CHECK, "wb+");
    fwrite(data, sizeof(char), length, tmp);
    fclose(tmp);
    pipe = popen("./crc " TEMP_FILE_FOR_CRC_CHECK, "r");
    //fscanf(pipe, "%as", crc);
    fscanf(pipe, "%s", crc);
    result->length = strlen(crc) / 2;
    result->crc = malloc(result->length);
    for(i = 0; i < result->length; i++) {
        sscanf(crc + i * 2, "%X", result->crc + i);
    }
    //free(crc);
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
