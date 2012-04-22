#ifndef __PACKET_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>

typedef struct {
    uint32_t header_length;     /*  4 bytes */
    uint32_t one_value;         /*  4 bytes */
    char hardware_id[8];        /*  8 bytes */
    uint32_t file_sequence;     /*  4 bytes */
    uint32_t data_file_length;  /*  4 bytes */
    char date[16];              /* 16 bytes */
    char time[16];              /* 16 bytes */
    char input_word[16];        /* 16 bytes */
    char blank[16];             /* 16 bytes */
    uint16_t header_crc;        /*  2 bytes */
    uint16_t one_value2;        /*  2 bytes */
    uint16_t blank2;            /*  2 bytes */
} packet_header_t;

typedef struct {
    packet_header_t header;     /* 94 bytes */
    char *crc;                  /* variable size */
    char *file_data;            /* variable size */
    char *filename;
    int crc_length;
    bool is_crc_ok;
} packet_t;

/* Parse one single packet (file) */
packet_t *
parse_next_file(FILE *input);

/* Read a file and convert to packet */
packet_t *
read_packet_file(FILE *input, const char *filename, struct stat sbuf);

/* Convert packet_t into a bytearray */
char *
packet_to_buf(const packet_t *packet, size_t *restrict length);

/* Write file separator */
FILE *
write_file_separator(FILE *output);

#endif /* __PACKET_H__ */
