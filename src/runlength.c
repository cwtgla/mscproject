#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FILELOC "../data/stripped.txt"
#define TESTFILELOC "../data/runlength.txt" //testfile with 13 lines expected to turn to 10
#define ILEN 150
#define JLEN 150
#define KLEN 90

struct entry {
	float value;
	uint count;
};

int main(void) {
	int limit = ILEN*JLEN*KLEN;
	//int limit = 13;

	//Read in FPs into an array
	FILE *contentFile;
	float *content = calloc(limit,sizeof(float));
	contentFile = fopen(FILELOC,"r");

	int i = 0;
	while (fscanf(contentFile, "%f", &content[i]) == 1) {
		i++;
	}

	//Init runlength array
	struct entry* entries = calloc(sizeof(struct entry),limit);
	entries[0].count = 1;
	entries[0].value = content[0];

//	for(i = 0; i < limit; i++) {
//		printf("val %.8f\n", content[i]);
//	}

	int entryIndex = 0;

	for(i=1; i < limit; i++) {
		if(entries[entryIndex].value == content[i]) { //if last value is same as next
			entries[entryIndex].count++;
		} else {
			entryIndex++;
			entries[entryIndex].value = content[i];
			entries[entryIndex].count = 1;
		}
	}	
	free(content);
	free(entries);
	printf("Size of runlength array: %d\n", entryIndex+1);
	printf("In bytes : %d ", (entryIndex+1)*sizeof(struct entry));
	return 0;
}



//grab 1st item and save it in entry 0 and the count being 1
//then for every other element, if value is the same as previous then increment count and move on,