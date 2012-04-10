#include "config.h"
#include <stdio.h>
#include <string.h>
#include "crc.h"

#define TEMP_FILE_FOR_CRC_CHECK "/tmp/crc_temp_checking"

char *
crc16(const char *data, int length)
{
    FILE *pipe;
    FILE *tmp;/*TODO: save data to TEMP_FILE_FOR_CRC_CHECK*/
    char *crc;
    char *bytearray;
    int i;
    int size;

    remove(TEMP_FILE_FOR_CRC_CHECK);
    tmp = fopen(TEMP_FILE_FOR_CRC_CHECK, "wb+");
    fwrite(data, sizeof(char), length, tmp);
    fclose(TEMP_FILE_FOR_CRC_CHECK);
    pipe = popen("./crc" TEMP_FILE_FOR_CRC_CHECK, "r");
    fscanf(pipe, "%as", crc);
    size = strlen(crc) / 2;
    bytearray = malloc(size);
    for(i = 0; i < size; i++) {
        sscanf(crc + i * 2, "%X", bytearray + i);
    }
    free(crc);
    fclose(pipe);
    return bytearray;
}
