#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

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

    /* TODO: open output and dump packets into it inside the loop */
    dir = opendir(input_directory);
    while((dir_entry = readdir(dir)) != NULL) {
        strcpy(full_path, input_directory);
        strcat(full_path, "/");
        strcat(full_path, dir_entry->d_name);
        file = fopen(full_path, "rb");
        stat(full_path, &sbuf);
        packet = read_packet_file(file, dir_entry->d_name, sbuf);
        fclose(file);
    }

    return -1; /* TODO: return OK when fully implemented */
}
