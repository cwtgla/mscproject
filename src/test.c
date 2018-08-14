#include <stdint.h>
#include <string.h>
#include <stdio.h>

struct compressedVal {
	char data[3];
};

int main() {
//	uint32_t *val1;
//	*val1 = 10;
//	uint32_t val2 = 4920;

//	void *intermediate = (void *) val1;
//	unsigned char *initialValue = (unsigned char *) intermediate;
//	printf("%s\n", initialValue);


	unsigned char bytes[4];
	uint32_t intv = 1235678887;

	bytes[0] = (intv >> 24) & 0xFF;
	bytes[1] = (intv >> 16) & 0xFF;
	bytes[2] = (intv >> 8) & 0xFF;
	bytes[3] = intv & 0xFF;

	printf("%x %x %x %x\n", (unsigned char)bytes[0],(unsigned char) bytes[1],(unsigned char) bytes[2],(unsigned char) bytes[3]);

	return 0;
}
