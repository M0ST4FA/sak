#include "kernel/uapi/limits.h"

#define PATHSERVER_FD (PROC_NR + 3)

int mkfifo(const char *pathname, int mode);
int open(const char *pathname, int flags);
