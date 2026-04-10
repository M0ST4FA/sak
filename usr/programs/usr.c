#include "usr.h"
#include "asm.h"
#include "fs.h"
#include "kernel/uapi/limits.h"
#include "lib.h"

void print_lover(void) {
	print_string("In user mode: beginning\n");
	kernel_yield();
	print_string("In user mode: second time\n");
	kernel_yield();
	print_string("In user mode: end\n");
	kernel_yield();

	// make sure if the task doesn't have anything to do, it keeps trapping into the kernel
	while (1)
		;
}

void lesser_print_lover(void) {
	print_string("In other task\n");
	while (1)
		;
}

void fork_lover(void) {
	int child_pid;
	char buf[BUF_SIZE];

	print_string("I'm about to have a child...\n");

	child_pid = fork();
	if (child_pid == 0) {
		print_string("Inside child. Never though I'm gonna make it, but here I'm!\n");

		while (1)
			;
	}

	inttostr(child_pid, buf);
	print_string("Having a child is easier than what I expected...PID: ");
	print_string(buf);
	print_char('\n');

	while (1)
		;
}

void pathserver(void) {
	char paths[PIPE_NR - PROC_NR - 3][PATH_MAX];
	int npaths = 0, i = 0;
	unsigned int plen = 0, replyfd = 0;
	char path[PATH_MAX];

	memcpy(paths[npaths++], "/sys/pathserver", sizeof("/sys/pathserver"));

	while (1) {
		read(PATHSERVER_FD, &replyfd, 4);
		read(PATHSERVER_FD, &plen, 4);
		read(PATHSERVER_FD, path, plen);

		if (!replyfd) { // Register a new path
			memcpy(paths[npaths++], path, plen);
			continue;
		}

		for (i = 0; i < npaths; i++)
			if (*paths[i] && strcmp(path, paths[i]) == 0) {
				i += 3;
				i += PROC_NR;
				write(replyfd, &i, 4);
				i = 0;
				break;
			}

		if (i >= npaths) {
			i = -1; // ENOTFOUND, i will be reset in the for loop next iteration
			write(replyfd, &i, 4);
		}
	};
}

void otherguy(void) {
	int fd = 0;
	unsigned int len = 0;
	char buf[20];
	mkfifo("/proc/0", 0);
	fd = open("/proc/0", 0);
	while (1) {
		read(fd, &len, 4);
		read(fd, buf, len);
		print_string(buf);
	}
}

void init(void) {
	int fd;

	if (!fork())
		pathserver();
	if (!fork())
		otherguy();

	fd = open("/proc/0", 0);

	while (1) {
		const int str_len = sizeof("Ping\n");
		const int buf_len = str_len + 4;
		char buf[sizeof("Ping\n") + 4] = {0};

		memcpy(buf, (char *)&str_len, 4);
		memcpy(buf + 4, "Ping\n", str_len);

		write(fd, buf, buf_len);
	}
}

__attribute__((section(".init"))) void (*init_address)(void) = init;
