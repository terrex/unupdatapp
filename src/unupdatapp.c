#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define FILE_SEPARATOR "\x55\xAA\x5A\xA5"

typedef struct {
    uint32_t header_length;     /*  4 bytes */
    uint32_t one_value;         /*  4 bytes */
    char hardware_id[8];        /*  8 bytes */
    uint32_t file_sequence;   /*  4 bytes */
    uint32_t data_file_length;  /*  4 bytes */
    char date[16];              /* 16 bytes */
    char time[16];              /* 16 bytes */
    char input_word[16];        /* 16 bytes */
    char blank[16];             /* 16 bytes */
    uint16_t header_crc;        /*  2 bytes */
    uint16_t one_value2;        /*  2 bytes */
    uint16_t blank2;             /*  2 bytes */
} packet_header_t;

typedef struct {
    packet_header_t header;        /* 94 bytes */
    char *crc;                  /* variable size */
    char *file_data;            /* variable size */
    char *filename;
} packet_t;

const char* guess_filename(uint32_t file_sequence) {
    switch(file_sequence) {
        case 0x10000000:
            return	"appsboot.mbn";
        case 0x20000000:
            return "file25.mbn";
        case 0x30000000:
            return "boot.img";
        case 0x40000000:
            return "system.img";
        case 0x50000000:
            return "userdata.img";
        case 0x60000000:
            return "recovery.img";
        case 0xC0000000:
            return "file11.mbn";
        case 0xD0000000:
            return "file08.mbn";
        case 0xE0000000:
            return "file05.mbn";
        case 0xF0000000:
            return "file04.mbn";
        case 0xF1000000:
            return "file07.mbn";
        case 0xF2000000:
            return "splash.raw565";
        case 0xF3000000:
            return "file01.mbn";
        case 0xF4000000:
            return "file02.mbn";
        case 0xF5000000:
            return "file14.mbn";
        case 0xF6000000:
            return "boot_versions.txt";
        case 0xF7000000:
            return "upgradable_versions.txt";
        case 0xF8000000:
            return "file09.mbn";
        case 0xF9000000:
            return "version.txt";
        case 0xFA000000:
            return "file20.mbn";
        case 0xFB000000:
            return "appsboothd.mbn";
        case 0xFC000000:
            return "file23.mbn";
        case 0xFD000000:
            return "file16.mbn";
        case 0xFE000000:
            return "file18.mbn";
        case 0xFF000000:
            return "file21.mbn";
    }
}

packet_t parse_file(FILE *input)
{
    packet_t packet;
    fread(&packet.header, sizeof(packet.header), 1, input);
    packet.crc = malloc(packet.header.header_length - 98);
    fread(packet.crc, 1, (packet.header.header_length - 98), input);
    packet.file_data = malloc(packet.header.data_file_length);
    printf("reading %d bytes\n", packet.header.data_file_length);
    fread(packet.file_data, 1, packet.header.data_file_length, input);
    packet.filename = guess_filename(packet.header.file_sequence);

    return packet;
}

int main(int argc, char *argv[])
{
    FILE *input = ((argc == 2 && strcmp(argv[1], "-") != 0) ? fopen(argv[1], "rb") : stdin);
    uint8_t word[4];
    packet_t packet;
    while (fread(&word, sizeof(word), 1, input)) {
        if (memcmp(word, FILE_SEPARATOR, sizeof(word)) == 0) {
            printf("found file separator, reading\n");
            packet = parse_file(input);
            printf("file read: %s\n", packet.filename);
            FILE* output = fopen(packet.filename, "w+");
            fwrite(&packet.file_data, 1, packet.header.data_file_length, output);
            fclose(output);
        }
    }
    fclose(input);

    return 0;
}

