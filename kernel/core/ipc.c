#include "ipc.h"
#include "types.h"

struct pipe_ringbuffer pipes[PIPE_NR];

void ipc_init(void) {
	size_t i;

	for (i = 0; i < PIPE_NR; i++)
		pipes[i].start = pipes[i].end = 0;
}
