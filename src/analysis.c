#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "zfp_example.h"

float globalMax = -10000.0f;
float globalMin = 10000.0f;

// struct rlEntry {
// 	float value;
// 	uint count;
// };



// void runlengthStats(float *values, int count) {
// 	struct rlEntry *entries = calloc(count, sizeof(struct rlEntry));
// 	entries[0].count = 1;
// 	entries[0].value = values[0];

// 	int i;
// 	int entryIndex = 0;
// 	for(i = 1; i < count; i++) {
// 		if(entries[entryIndex].value == values[i]) {
// 			entries[entryIndex].count++;
// 		} else {
// 			entryIndex++;
// 			entries[entryIndex].value = values[i];
// 			entries[entryIndex].count = 1;
// 		}
// 	}

// 	free(entries);
// 	printf("\t\t*****RUNLENGTH STATS *****\n");
// 	printf("\t\tNumber of indexes in runlength array %d, a net reduction in %d indexes\n\t\tSize in bytes of runlength array %lu bytes\n", entryIndex, count-entryIndex, sizeof(struct rlEntry)*entryIndex);
// }

//Print stats from using zfp to compress values
void zfpStats(float *values, int count) {
	printf("\t\t\t*****ZFP STATS *****\n");
	size_t compressedSize;
	compressedSize = compress(values, 150, 150, 90, 0.00, 0);
	printf("\t\t\tSize after zfp compression %lu bytes\n", compressedSize);
}

//Get basic file stats (max,min,mean and number of values)
void displayStats(char *filePath) {
	FILE *contentFile;
	float min = 10000.0f;
	float max = -10000.0f;
	float currentVal;
	unsigned int count = 0;
	float total = 0.0f;

	contentFile = fopen(filePath, "r");
	float *content = calloc(150*150*90, sizeof(float));
	int i = 0;

	while(fscanf(contentFile, "%f", &content[i]) == 1) {
		total+=content[i];
		count++;
	
		if(content[i] > max)
			max = content[i];
		if(content[i] < min)
			min = content[i];
		i++;
	}
	fclose(contentFile);
	if(globalMax < max)
		globalMax = max;
	if(globalMin > min)
		globalMin = min;

	printf("*****STATS*****\n");
	printf("\tFor file: %s\n\tMax: %f\n\tMin: %f\n\tMean value: %f\n\tTotal number of values: %d\n\tStorage size as is: %lu bytes\n", filePath, max, min, (total/count), count, count*sizeof(float));
	
	runlengthStats(content, count);
	zfpStats(content, count);

	free(content);
}

	int main() {
		//grab all files data
		//for each file
		//perform runlength encoding, write out new sizes
		//perform 24 bit compression, write out new sizes
		//perform 24 bit non aligned compression


		//compression performance
		//compress data?
		//run transformation
		return 0;
	}















// int main(int argc, char *argv[]) {
// 	if(argc < 2) {
// 		printf("Missing additional params, recieved: %d\n", argc);
// 		return -1;
// 	}
// 	printf("Getting files in %s\n", argv[1]);
// 	//Grab absolute path of all relevant files
// 	char *files[10];
// 	getAbsFilenames(argv[1], files, ".txt.clean");
	
// 	int i = 0;
// 	printf("Found files..\n");
// 	for(i = 0; i < 6; i++) {
// 		printf("%d : %s\n", i+1, files[i]);
// 	}

// 	printf("Getting stats for files..\n");
// 	//Get each files stats
// 	for(i = 0; i < 6; i++) {
// 		displayStats(files[i]);
// 	}

// 	printf("Global stats..\n");
// 	printf("Largest value found across files: %f. Smallest value found: %f\n", globalMax, globalMin);

	//struct dirent *directoryEntry;
	//DIR *directory = opendir(argv[1]);

	//if(directory == NULL) {
//		printf("ERROR");
//		return -1;
//	}
	
	//read entries
//	while((directoryEntry = readdir(directory)) != NULL) {
//		printf("%s\n", directoryEntry->d_name);
//	}
//	closedir(directory);
	return 0;
}
