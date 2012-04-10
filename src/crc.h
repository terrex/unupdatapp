#ifndef __CRC_H__
#define __CRC_H__

typedef struct {
    char *crc;
    size_t length;
} crc_t;

/* calculates crc16 for data stream */
crc_t *
crc16(const char *data, int length);

crc_t *
new_crc_t(const char *crc, const size_t length);

#endif /* __CRC_H__ */
