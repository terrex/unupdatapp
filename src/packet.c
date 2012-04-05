#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "packet.h"

/* private declarations */

typedef struct {
    uint32_t file_sequence;
    const char *filename;
} file_sequence_pair_t;

static const char* guess_filename(uint32_t file_sequence);

static void fseek_align4(FILE *input);

/* private definitions */

static file_sequence_pair_t FILE_SEQS[] = {
    { 0x10000000, "appsboot.mbn" },
    { 0x20000000, "file25.mbn" },
    { 0x30000000, "boot.img" },
    { 0x40000000, "system.img" },
    { 0x50000000, "userdata.img" },
    { 0x60000000, "recovery.img" },
    { 0xC0000000, "file11.mbn" },
    { 0xD0000000, "file08.mbn" },
    { 0xE0000000, "file05.mbn" },
    { 0xF0000000, "file04.mbn" },
    { 0xF1000000, "file07.mbn" },
    { 0xF2000000, "splash.raw565" },
    { 0xF3000000, "file01.mbn" },
    { 0xF4000000, "file02.mbn" },
    { 0xF5000000, "file14.mbn" },
    { 0xF6000000, "boot_versions.txt" },
    { 0xF7000000, "upgradable_versions.txt" },
    { 0xF8000000, "file09.mbn" },
    { 0xF9000000, "version.txt" },
    { 0xFA000000, "file20.mbn" },
    { 0xFB000000, "appsboothd.mbn" },
    { 0xFC000000, "file23.mbn" },
    { 0xFD000000, "file16.mbn" },
    { 0xFE000000, "file18.mbn" },
    { 0xFF000000, "file21.mbn" }
};

static int UNKNOWN_COUNT = 0;

static const char* guess_filename(uint32_t file_sequence)
{
    int i;
    char *result;
    
    for(i = 0; i < sizeof(FILE_SEQS); i++) {
        if(file_sequence == FILE_SEQS[i].file_sequence) {
            return FILE_SEQS[i].filename;
        }
    }
    
    result = calloc(sizeof(char), 20);
    sprintf(result, "unknown_file.%d", UNKNOWN_COUNT++);
    return (const char*)result;
}

static void fseek_align4(FILE *input)
{
    fpos_t position;

    fgetpos(input, &position);
    //fseek(input, position.__pos % 4, SEEK_CUR);
    switch((position.__pos) % 4) {
        case 0:
            break;
        case 1:
            fgetc(input);
            fgetc(input);
            fgetc(input);
            break;
        case 2:
            fgetc(input);
            fgetc(input);
            break;
        case 3:
            fgetc(input);
            break;
    }
}

/* public definitions */

void start_read_packets()
{
    UNKNOWN_COUNT = 0;
}

packet_t parse_next_file(FILE *input)
{
    packet_t packet;

    fread(&packet.header.header_length, sizeof(packet.header.header_length), 1, input);
    fread(&packet.header.one_value, sizeof(packet.header.one_value), 1, input);
    fread(packet.header.hardware_id, sizeof(packet.header.hardware_id), 1, input);
    fread(&packet.header.file_sequence, sizeof(packet.header.file_sequence), 1, input);
    fread(&packet.header.data_file_length, sizeof(packet.header.data_file_length), 1, input);
    fread(packet.header.date, sizeof(packet.header.date), 1, input);
    fread(packet.header.time, sizeof(packet.header.time), 1, input);
    fread(packet.header.input_word, sizeof(packet.header.input_word), 1, input);
    fread(packet.header.blank, sizeof(packet.header.blank), 1, input);
    fread(&packet.header.header_crc, sizeof(packet.header.header_crc), 1, input);
    fread(&packet.header.one_value2, sizeof(packet.header.one_value2), 1, input);
    fread(&packet.header.blank2, sizeof(packet.header.blank2), 1, input);
    packet.crc = malloc(packet.header.header_length - 98);
    fread(packet.crc, 1, (packet.header.header_length - 98), input);
    packet.file_data = malloc(packet.header.data_file_length);
    fread(packet.file_data, 1, packet.header.data_file_length, input);
    fseek_align4(input);
    packet.filename = (char*)guess_filename(packet.header.file_sequence);

    return packet;
}
