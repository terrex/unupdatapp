#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FILE_SEPARATOR "\x55\xAA\x5A\xA5"

#ifdef HAVE_DEBUG
void print_bytes(void *value, size_t n)
{
    if(n == 1) {
        printf(" %02X", (*(char*)value) & 0x000000FF);
    } else if(n == 2) {
        printf(" %02X", (*(char*)value) & 0x000000FF);
        printf(" %02X", (*(char*)(value + 1)) & 0x000000FF);
    } else if(n == 3) {
        printf(" %02X", (*(char*)value) & 0x000000FF);
        printf(" %02X", (*(char*)(value + 1)) & 0x000000FF);
        printf(" %02X", (*(char*)(value + 2)) & 0x000000FF);
    } else if(n == 4) {
        printf(" %02X", (*(char*)value) & 0x000000FF);
        printf(" %02X", (*(char*)(value + 1)) & 0x000000FF);
        printf(" %02X", (*(char*)(value + 2)) & 0x000000FF);
        printf(" %02X", (*(char*)(value + 3)) & 0x000000FF);
    } else if(n > 4) {
        print_bytes(value, 4);
        print_bytes(value + 4, n - 4);
    }
}
#define PRINT_GET_POS(f) { fpos_t tam; fgetpos((f), &tam); printf("current offset %d\n", tam); }
#define READED(val) { printf("data read:"); print_bytes(&(val), sizeof(val)); printf("\n"); }
#define READED_PTR(val, tam) { printf("data read (%d bytes long):", (tam)); print_bytes((val), (tam)); printf("\n"); }
#else /* HAVE_DEBUG */
#define PRINT_GET_POS(f)
#define READED(val)
#define READED_PTR(val, tam)
#endif /* HAVE_DEBUG */

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

static const char* guess_filename(uint32_t file_sequence) {
    static int unknown_count = 0;
    char *result;

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
        default:
            result = calloc(sizeof(char), 20);
            sprintf(result, "unknown_file.%d", unknown_count++);
            return (const char*)result;
    }
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

packet_t parse_file(FILE *input)
{
    packet_t packet;

    fread(&packet.header.header_length, sizeof(packet.header.header_length), 1, input);
READED(packet.header.header_length)
    fread(&packet.header.one_value, sizeof(packet.header.one_value), 1, input);
READED(packet.header.one_value)
    fread(packet.header.hardware_id, sizeof(packet.header.hardware_id), 1, input);
READED(packet.header.hardware_id)
    fread(&packet.header.file_sequence, sizeof(packet.header.file_sequence), 1, input);
READED(packet.header.file_sequence)
    fread(&packet.header.data_file_length, sizeof(packet.header.data_file_length), 1, input);
READED(packet.header.data_file_length)
    fread(packet.header.date, sizeof(packet.header.date), 1, input);
READED(packet.header.date)
    fread(packet.header.time, sizeof(packet.header.time), 1, input);
READED(packet.header.time)
    fread(packet.header.input_word, sizeof(packet.header.input_word), 1, input);
READED(packet.header.input_word)
    fread(packet.header.blank, sizeof(packet.header.blank), 1, input);
READED(packet.header.blank)
    fread(&packet.header.header_crc, sizeof(packet.header.header_crc), 1, input);
READED(packet.header.header_crc)
    fread(&packet.header.one_value2, sizeof(packet.header.one_value2), 1, input);
READED(packet.header.one_value2)
    fread(&packet.header.blank2, sizeof(packet.header.blank2), 1, input);
READED(packet.header.blank2)
    packet.crc = malloc(packet.header.header_length - 98);
    fread(packet.crc, 1, (packet.header.header_length - 98), input);
//READED_PTR(packet.crc, packet.header.header_length - 98)
    packet.file_data = malloc(packet.header.data_file_length);
    printf("reading %d bytes\n", packet.header.data_file_length);
    fread(packet.file_data, 1, packet.header.data_file_length, input);
//READED_PTR(packet.file_data, packet.header.data_file_length)
    fseek_align4(input);
    packet.filename = (char*)guess_filename(packet.header.file_sequence);

    return packet;
}

int main(int argc, char *argv[])
{
    FILE *input;
    char destination[255];
    
    input = fopen(argv[1], "rb");
    mkdir(argv[2], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    uint8_t word[4];
    packet_t packet;
    while (fread(word, sizeof(word), 1, input)) {
        if (memcmp(word, FILE_SEPARATOR, sizeof(word)) == 0) {
READED_PTR(word, 4)
            printf("found file separator, reading\n");
            packet = parse_file(input);
            printf("file read: %s\n", packet.filename);
            destination[0] = '\0';
            strcat(destination, argv[2]);
            strcat(destination, "/");
            strcat(destination, packet.filename);
            FILE* output = fopen(destination, "w+");
            fwrite(packet.file_data, 1, packet.header.data_file_length, output);
            fclose(output);
        }
    }
    fclose(input);

    return 0;
}

