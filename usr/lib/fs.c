#include "fs.h"
#include "asm.h"
#include "kernel/uapi/limits.h"
#include "kernel/uapi/types.h"
#include "lib.h"

int mkfifo(const char *pathname, int mode) {
	size_t plen = strlen(pathname);
	char buf[4 + 4 + PATH_MAX];
	(void)mode;

	*((unsigned int *)buf) = 0;
	*((unsigned int *)(buf + 4)) = plen;
	memcpy(buf + 4 + 4, pathname, plen);
	write(PATHSERVER_FD, buf, 4 + 4 + plen);

	return 0;
}

int open(const char *pathname, int flags) {
	pid_t replyfd = getpid() + 3;
	size_t plen = strlen(pathname) + 1;
	unsigned int fd = -1;

	char buf[4 + 4 + PATH_MAX];
	(void)flags;

	*((unsigned int *)buf) = replyfd;
	*((unsigned int *)(buf + 4)) = plen;
	memcpy(buf + 4 + 4, pathname, plen);
	write(PATHSERVER_FD, buf, 4 + 4 + plen);
	read(replyfd, &fd, 4);

	return fd;
}
