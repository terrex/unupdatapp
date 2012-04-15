#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "popen2.h"
#include "crc.h"

crc_t *
crc16(const char *data, int length)
{
    crc_t *result;
    int infp;
    int outfp;
    int i;
    unsigned int temp_read;
    int expected_length;
    char temp[3];
    ssize_t written;

    result = calloc(sizeof(crc_t), 1);
    expected_length = (length + (length % 4096)) / 4096 * 2;
    result->crc = malloc(expected_length + 1);
    popen2("./crc /dev/stdin", &infp, &outfp);

    temp[2] = '\0';
    i = 0;
    written = 0;

    write(infp, data, length);
    close(infp);
    while(read(outfp, temp, 2) == 2) {
        sscanf(temp, "%02X", &temp_read);
        result->crc[i] = (char)temp_read;
        i++;
    }

    result->crc[i] = '\0';
    result->length = i;

//    close(infp);
    close(outfp);

    return result;
}

crc_t *
new_crc_t(const char *crc, const size_t length)
{
    crc_t *result;

    result = malloc(sizeof(crc_t));
    result->crc = (char *)crc;
    result->length = length;

    return result;
}

bool
crc_are_equal(const crc_t *a, const crc_t *b)
{
    return a->length == b->length && memcmp(a->crc, b->crc, a->length) == 0;
}
