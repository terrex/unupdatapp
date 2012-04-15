#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <libgen.h>
#include <string.h>

#include "updatapp.h"
#include "unupdatapp.h"

int
main(const int argc, const char *argv[])
{
    if(strcmp(basename((char *)argv[0]), "unupdatapp") == 0 && argc == 3) {
        return unupdatapp(argv[1], argv[2]);
    } else if(strcmp(basename((char *)argv[0]), "updatapp") == 0 && argc == 3) {
        return updatapp(argv[1], argv[2]);
    }

    return -1;
}
