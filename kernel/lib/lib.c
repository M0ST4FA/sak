#include "lib.h"
#include "arch/versatilepb.h"
#include "types.h"

void print_char(char c) {
	*UART0 = c;
}

void print_string(const char *str) {
	while (*str)
		print_char(*str++);
}

void print_int(int i) {
	char buf[BUF_SIZE] = {0};

	inttostr(i, buf);
	print_string(buf);
}

int add(int a, int b) {
	return a + b;
}

void *memcpy(char *dst, char *src, int n) {
	char *d = dst, *s = src;
	size_t i;

	for (i = 0; i < n; i++)
		d[i] = s[i];

	return d;
}

void *memset(char *dst, char val, int n) {
	char *d = dst;
	size_t i;

	for (i = 0; i < n; i++)
		d[i] = val;

	return d;
}

void reverse(char *buf, int size) {
	int start = 0;
	int end = size - 1;
	int temp;

	while (start < end) {
		// swap elements at start and end
		temp = buf[start];
		buf[start] = buf[end];
		buf[end] = temp;

		// move pointers towards the middle
		start++;
		end--;
	}
}

int inttostr(int i, char *buf) {
	int div = i, rem = 0, c = 0;

	if (i == 0) {
		buf[c++] = '0';
		buf[c] = '\0';
		return 0;
	}

	do {
		rem = div % 10;
		buf[c++] = rem + '0';

		div /= 10;
	} while (div > 0);

	buf[c] = '\0';

	reverse(buf, c);

	return 0;
}

int strcmp(const char *a, const char *b) {
	int r = 0;

	while (!r && *a && *b)
		r = (*a++) - (*b++);

	return (*a) - (*b);
}

size_t strlen(const char *s) {
	size_t r = 0;

	while (*(s + r++))
		;

	return r;
}
