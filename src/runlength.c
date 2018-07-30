#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FILELOC "../data/simulation_data/one_half_sim.txt.clean"
//#define FILELOC "../data/simulation_data/complete_sim.txt.clean"
//#define FILELOC "../data/simulation_data/one_quarter_sim.txt.clean"
//#define FILELOC "../data/simulation_data/one_third_sim.txt.clean"
//#define FILELOC "../data/simulation_data/three_quarter_sim.txt.clean"
//#define FILELOC "../data/simulation_data/two_third_sim.txt.clean"
#define LINECOUNT 2025000

struct entry {
	float value;
	uint count;
};

int main(void) {
	//read floating points into array
	FILE *contentFile;
	float *content = calloc(LINECOUNT, sizeof(float));
	contentFile = fopen(FILELOC, "r");

	//while not EOF
	int i = 0;
	while (fscanf(contentFile, "%f", &content[i]) == 1) {
		i++;
	}

	//Init runlength array
	struct entry* entries = calloc(LINECOUNT, sizeof(struct entry));
	entries[0].count = 1;
	entries[0].value = content[0];

	int entryIndex = 0;
	for(i=1; i < LINECOUNT; i++) {
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
	printf("In bytes : %lu ", (entryIndex+1)*sizeof(struct entry));

	return 0;
}
