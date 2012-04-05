#include "config.h"
#include <stdio.h>
#include <string.h>
#include "crc.h"

#define TEMP_FILE_FOR_CRC_CHECK "/tmp/crc_temp_checking"

char *
crc16(const char *data, int length)
{
    FILE *pipe;/*TODO: save data to TEMP_FILE_FOR_CRC_CHECK*/
    char *crc;
    char *bytearray;
    int i;
    int size;

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
