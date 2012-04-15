#ifndef __POPEN2_H__
#define __POPEN2_H__

#include <sys/types.h>

pid_t
popen2(const char *command, int *infp, int *outfp);
#endif /* __POPEN2_H__ */
