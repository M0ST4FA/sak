#pragma once

#define EBADF 0x0
#define EAGAIN 0x1
#define ENOMEM 0x2
#define EMSGSIZE 0x3

extern void print_err(unsigned int err_code);
