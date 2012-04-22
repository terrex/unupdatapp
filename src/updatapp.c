#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "updatapp.h"
#include "packet.h"

int
updatapp(const char *input_directory, const char *output_file)
{
    DIR *dir;
    struct dirent *dir_entry;
    packet_t *packet;
    FILE *file;
    char full_path[2000];
    struct stat sbuf;
    FILE *output;
    char *packet_buf;
    size_t buf_length;
    int i;

    output = fopen(output_file, "wb+");
    /* write 96 zero padding */
    i = 0;
    while(i < 92) {
        fputc(0, output);
        i++;
    }
    dir = opendir(input_directory);
    while((dir_entry = readdir(dir)) != NULL) {
        if(strcmp(dir_entry->d_name, ".") == 0 ||
            strcmp(dir_entry->d_name, "..") == 0) {
            continue;
        }
        write_file_separator(output);
        strcpy(full_path, input_directory);
        strcat(full_path, "/");
        strcat(full_path, dir_entry->d_name);
        file = fopen(full_path, "rb");
        stat(full_path, &sbuf);
        packet = read_packet_file(file, dir_entry->d_name, sbuf);
        fclose(file);
        printf("Adding %s to %s\n", packet->filename, output_file);
        packet_buf = packet_to_buf(packet, &buf_length);
        fwrite(packet_buf, 1, buf_length, output);
        if(ftell(output) % 4 != 0) {
            printf("AAAHMIGO\n");
            printf("AAAHMIGO\n");
        }
        free(packet_buf);
        free(packet->crc);
        free(packet->file_data);
        free(packet);
    }
    fclose(output);

    return 0;
}
