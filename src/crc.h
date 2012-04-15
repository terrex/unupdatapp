#ifndef __CRC_H__
#define __CRC_H__

#include <stdbool.h>

typedef struct {
    char *crc;
    size_t length;
} crc_t;

/* calculates crc16 for data stream */
crc_t *
crc16(const char *data, int length);

crc_t *
new_crc_t(const char *crc, const size_t length);

bool
crc_are_equal(const crc_t *a, const crc_t *b);

#endif /* __CRC_H__ */
