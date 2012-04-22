#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "packet.h"
#include "crc.h"

/* private declarations */

typedef struct {
    uint32_t file_sequence;
    const char *filename;
} file_sequence_pair_t;

static const char* guess_filename(uint32_t file_sequence);

static uint32_t guess_fileseq(const char *filename);

int file_size(FILE *file);

static void fseek_align4(FILE *input);

/* private definitions */

#define FILE_SEPARATOR "\x55\xAA\x5A\xA5"

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
    { 0xFF000000, "file21.mbn" },
    { 0xF2400000, "partition_table.mbn" }, /* maybe? http://android-dls.com/wiki/index.php?title=How_to_unpack_and_repack_NB0_file */
    { 0xF2500000, "qcsbl.mbn" }, /* https://docs.symbol.com/ReleaseNotes/Release%20Notes%20-%20MC659BOS23001.htm */
    { 0xF6100000, "version_boot.txt"} /* guessed: this is the first boot version
          listed in boot_versions.txt. To be self-upgradable. */
};

static int UNKNOWN_COUNT = 0;

static const char* guess_filename(uint32_t file_sequence)
{
    size_t i;
    char *result;

    for(i = 0; i < sizeof(FILE_SEQS) / sizeof(FILE_SEQS[0]); i++) {
        if(file_sequence == FILE_SEQS[i].file_sequence) {
            return FILE_SEQS[i].filename;
        }
    }

    result = calloc(sizeof(char), 20);
    printf("unknown_file.%d has a file_seq of: 0x%08X\n", UNKNOWN_COUNT, file_sequence);
    sprintf(result, "unknown_file.%d", UNKNOWN_COUNT++);
    return (const char*)result;
}

static uint32_t guess_fileseq(const char *filename)
{
    size_t i;

    for(i = 0; i < sizeof(FILE_SEQS) / sizeof(FILE_SEQS[0]); i++) {
        if(strcmp(filename, FILE_SEQS[i].filename) == 0) {
            return FILE_SEQS[i].file_sequence;
        }
    }

    printf("Error: unknown file sequence for file %s\n", filename);
    return 0;
}

static void fseek_align4(FILE *input)
{
    fpos_t position;

    fgetpos(input, &position);
    switch((position.__pos) % 4) { /* perhaps .__pos should be changed to
                                       another more portable code */
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

int file_size(FILE *file)
{
    int size;
    int orig;

    orig = ftell(file);
    /* http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c#answer-238609 */
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, orig, SEEK_SET);

    return size;
}

/* public definitions */

packet_t *parse_next_file(FILE *input)
{
    uint8_t word[4];
    packet_t *packet;
    static FILE *last_input;

    if(input != last_input) {
        last_input = input;
        UNKNOWN_COUNT = 0;
    }

    packet = malloc(sizeof(packet_t));

    /* consume input until FILE_SEPARATOR has been found */
    while(fread(word, sizeof(word), 1, input)) {
        if (memcmp(word, FILE_SEPARATOR, sizeof(word)) == 0) {
            break;
        }
    }

    if(feof(input)) {
        return NULL;
    }

    /* start read of fixed sized header (94 bytes long) */
    fread(&packet->header.header_length, sizeof(packet->header.header_length), 1, input);
    fread(&packet->header.one_value, sizeof(packet->header.one_value), 1, input);
    fread(packet->header.hardware_id, sizeof(packet->header.hardware_id), 1, input);
    fread(&packet->header.file_sequence, sizeof(packet->header.file_sequence), 1, input);
    fread(&packet->header.data_file_length, sizeof(packet->header.data_file_length), 1, input);
    fread(packet->header.date, sizeof(packet->header.date), 1, input);
    fread(packet->header.time, sizeof(packet->header.time), 1, input);
    fread(packet->header.input_word, sizeof(packet->header.input_word), 1, input);
    fread(packet->header.blank, sizeof(packet->header.blank), 1, input);
    fread(&packet->header.header_crc, sizeof(packet->header.header_crc), 1, input);
    fread(&packet->header.one_value2, sizeof(packet->header.one_value2), 1, input);
    fread(&packet->header.blank2, sizeof(packet->header.blank2), 1, input);

    /* start read of variable sized CRC & data */
    packet->crc_length = packet->header.header_length - 98;
    packet->crc = malloc(packet->crc_length);
    fread(packet->crc, 1, packet->crc_length, input);
    packet->file_data = malloc(packet->header.data_file_length);
    fread(packet->file_data, 1, packet->header.data_file_length, input);
    packet->filename = (char*)guess_filename(packet->header.file_sequence);

    /* seek file up to next 4-byte boundary alignment */
    fseek_align4(input);

    /* test for crc ok */
    packet->is_crc_ok = crc_are_equal(new_crc_t(packet->crc, packet->crc_length), crc16(packet->file_data, packet->header.data_file_length));

    return packet;
}

packet_t *
read_packet_file(FILE *input, const char *filename, struct stat sbuf)
{
    packet_t *result;
    struct tm *tm;
    crc_t *header_crc;
    crc_t *file_crc;

    result = calloc(sizeof(packet_t), 1);
    result->header.one_value = (uint32_t) 1;
    strncpy(result->header.hardware_id, "HW7x25\xFF\xFF", 8);
    result->header.file_sequence = guess_fileseq(filename);
    result->header.data_file_length = (uint32_t) file_size(input);
    tm = localtime(&sbuf.st_mtime);
    strftime(result->header.date, 16, "%Y.%m.%d", tm);
    strftime(result->header.time, 16, "%H.%M.%S", tm);
    strcpy(result->header.input_word, "INPUT");
    result->header.one_value2 = (uint32_t) 1;
    result->file_data = calloc(1, result->header.data_file_length);
    fread(result->file_data, 1, result->header.data_file_length, input);
    file_crc = crc16(result->file_data, result->header.data_file_length);
    result->crc = file_crc->crc;
    result->filename = filename;
    result->crc_length = file_crc->length;
    result->is_crc_ok = true;
    /* TODO: computes and fills header crc */
    result->header.header_length = 98 + result->crc_length;

    return result;
}

/* Convert packet_t into a bytearray */
char *
packet_to_buf(const packet_t *packet, size_t *restrict buf_length)
{
    char *result;
    size_t length;
    size_t padding;

    length = packet->header.header_length - 4 + packet->header.data_file_length;
    padding = length % 4; /* pad zeroes until next 4-byte boundary */
    result = malloc(length + padding);
    memcpy(result, &(packet->header), 94);
    memcpy(result + 94, packet->crc, packet->crc_length);
    memcpy(result + 94 + packet->crc_length, packet->file_data, packet->header.data_file_length);
    memset(result + 94 + packet->crc_length + packet->header.data_file_length, 0, padding);

    *buf_length = length + padding;
    return result;
}


FILE *
write_file_separator(FILE *output)
{
    fwrite(FILE_SEPARATOR, 4, 1, output);

    return output;
}
