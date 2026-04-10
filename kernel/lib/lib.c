#include "lib.h"
#include "arch/versatilepb.h"
#include "uapi/types.h"

void kprint_char(char c) {
	*UART0 = c;
}

void kprint_string(const char *str) {
	while (*str)
		kprint_char(*str++);
}

void kprint_int(int i) {
	char buf[BUF_SIZE] = {0};

	kinttostr(i, buf);
	kprint_string(buf);
}

void *kmemcpy(char *dst, char *src, int n) {
	char *d = dst, *s = src;
	ssize_t i;

	for (i = 0; i < n; i++)
		d[i] = s[i];

	return d;
}

void *kmemset(char *dst, char val, int n) {
	char *d = dst;
	ssize_t i;

	for (i = 0; i < n; i++)
		d[i] = val;

	return d;
}

void kreverse(char *buf, int size) {
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

int kinttostr(int i, char *buf) {
	int div = i, rem = 0, c = 0, neg = 0;

	if (i < 0) {
		neg = 1;
		div = -i;
	}

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

	if (neg)
		buf[c++] = '-';

	buf[c] = '\0';

	kreverse(buf, c);

	return 0;
}

int kstrcmp(const char *a, const char *b) {
	int r = 0;

	while (!r && *a && *b)
		r = (*a++) - (*b++);

	return (*a) - (*b);
}

size_t kstrlen(const char *s) {
	size_t r = 0;

	while (*(s + r++))
		;

	return r;
}
