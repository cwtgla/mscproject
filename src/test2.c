#include <stdint.h>
#include <string.h>
#include <stdio.h>

struct compressedVal {
	char data[3];
};

int main() {
	uint32_t a = 1234567855;
	void *ptr = &a;
	char *ptr2 = (char *)(ptr);
	printf("%hhx %hhx %hhx %hhx\n", ptr2[0], ptr2[1], ptr2[2], ptr2[3]);

	printf("%hhx\n", ((char *) ((void *) a)));
	return 0;
}
