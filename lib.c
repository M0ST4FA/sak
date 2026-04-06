#include "lib.h"
#include "versatilepb.h"

void print_char(char c) {
	*UART0 = c;
}

void print_string(const char *str) {
	while (*str)
		print_char(*str++);
}

int add(int a, int b) {
	return a + b;
}

void memcpy(char *dst, char *src, int n) {
	for (int i = 0; i < n; i++)
		*(dst + i) = *(src + i);
}

void memset(char *dst, char val, int n) {
	for (int i = 0; i < n; i++)
		dst[i] = val;
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
