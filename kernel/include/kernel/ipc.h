#pragma once

#include "task.h"
#include "uapi/limits.h"

struct pipe_ringbuffer {
	int start;
	int end;
	char data[PIPE_BUF_SZ];
};

#define rb_push(rb, size, v)         \
	do {                             \
		(rb)->data[(rb)->end] = (v); \
		(rb)->end++;                 \
		if ((rb)->end > size)        \
			(rb)->end = 0;           \
	} while (0)

#define rb_pop(rb, size, v)             \
	do {                                \
		*(v) = (rb)->data[(rb)->start]; \
		(rb)->start++;                  \
		if ((rb)->start > size)         \
			(rb)->start = 0;            \
	} while (0)

#define rb_len(rb, size) ((rb)->end - (rb)->start + \
						  (((rb)->end < (rb)->start) ? size : 0))

#define pipe_push(pipe, v) rb_push((pipe), PIPE_BUF_SZ, (v))
#define pipe_pop(pipe, v) rb_pop((pipe), PIPE_BUF_SZ, (v))
#define pipe_len(pipe) (rb_len((pipe), PIPE_BUF_SZ))

extern struct pipe_ringbuffer pipes[PIPE_NR];

extern void ipc_init(void);
