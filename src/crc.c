#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "popen2.h"
#include "crc.h"

typedef struct {
    int infp;
    const char *data;
    int length;
} pipe_writer_args_t;

static void *
pipe_writer(void *_args)
{
    pipe_writer_args_t *args = (pipe_writer_args_t *) _args;
    write(args->infp, args->data, args->length);
    close(args->infp);
    return NULL;
}

crc_t *
crc16(const char *data, const int length)
{
    crc_t *result;
    int infp;
    int outfp;
    int i;
    unsigned int temp_read;
    char temp[2];
    pthread_t writer;
    pipe_writer_args_t pipe_writer_args;

    result = calloc(sizeof(crc_t), 1);
    result->length = ((length % 4096) ? length / 4096 + 1 : length / 4096) * 2;
    result->crc = malloc(result->length);
    popen2("./crc /dev/stdin", &infp, &outfp);

    pipe_writer_args.infp = infp;
    pipe_writer_args.data = data;
    pipe_writer_args.length = length;
    pthread_create(&writer, NULL, pipe_writer, (void *)&pipe_writer_args);
    i = 0;
    while(read(outfp, temp, 2) == 2) {
        sscanf(temp, "%02X", &temp_read);
        result->crc[i] = (char)temp_read;
        i++;
    }
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
