#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "packet.h"

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

/* main program entry point */
int main(int argc, char *argv[])
{
    FILE *input;
    char destination[255];
    packet_t *packet;

    input = fopen(argv[1], "rb");
    start_read_packets();
    mkdir(argv[2], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    while(packet = parse_next_file(input)) {
        printf("Extracted %s - ", packet->filename);
        if(packet->is_crc_ok) {
            printf("CRC Okay\n");
        } else {
            printf("CRC Failed!\n");
        }
        destination[0] = '\0';
        strcat(destination, argv[2]);
        strcat(destination, "/");
        strcat(destination, packet->filename);
        FILE* output = fopen(destination, "w+");
        fwrite(packet->file_data, 1, packet->header.data_file_length, output);
        fclose(output);
    }
    fclose(input);

    return 0;
}
