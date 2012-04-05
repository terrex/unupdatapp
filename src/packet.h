#ifndef __PACKET_H__

#include <stdio.h>
#include <stdint.h>

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
} packet_t;

/* Parse one single packet (file) */
packet_t *parse_next_file(FILE *input);

/* To be called when I am going to parse a new UPDATA.APP */
void start_read_packets();

#endif /* __PACKET_H__ */
