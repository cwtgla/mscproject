#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

float globalMax = -10000.0f;
float globalMin = 10000.0f;

//assumes 0'd out files array
void getAbsFilenames(char* basedir, char* files[], char* extension) {
	struct dirent *directoryEntry;
	DIR *directory = opendir(basedir);
	int pathLen = strlen(basedir)+1; //for null char

	if(directory != NULL) {
		int count = 0;
		//whilst still iterating over an entry
		while((directoryEntry = readdir(directory)) != NULL) {
			//if the entry is of the desired extension
			if(strstr(directoryEntry->d_name,extension) != NULL) {
				files[count] = calloc((strlen(directoryEntry->d_name)+pathLen), sizeof(char));
				strcat(files[count], basedir);
				strcat(files[count], directoryEntry->d_name);
				count++;
			}
		}
	}
}

void displayStats(char* filePath) {
	FILE *contentFile;
	float min = 10000.0f;
	float max = -10000.0f;
	float currentVal;
	unsigned int count = 0;
	float total = 0.0f;

	contentFile = fopen(filePath, "r");
	while(fscanf(contentFile, "%f", &currentVal) == 1) {
		total+=currentVal;
		count++;

		if(currentVal > max)
			max = currentVal;
		if(currentVal < min)
			min = currentVal;
	}
	fclose(contentFile);
	if(globalMax < max)
		globalMax = max;
	if(globalMin > min)
		globalMin = min;

	printf("*****STATS*****\n");
	printf("For file: %s\nMax: %f\nMin: %f\nMean value: %f\nTotal number of values: %d\n", filePath, max, min, (total/count), count);
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		printf("Missing additional params, recieved: %d\n", argc);
		return -1;
	}
	printf("Getting files in %s\n", argv[1]);
	//Grab absolute path of all relevant files
	char *files[10];
	getAbsFilenames(argv[1], files, ".txt.clean");
	
	int i = 0;
	printf("Found files..\n");
	for(i = 0; i < 6; i++) {
		printf("%d : %s\n", i+1, files[i]);
	}

	printf("Getting stats for files..\n");
	//Get each files stats
	for(i = 0; i < 6; i++) {
		displayStats(files[i]);
	}

	printf("Global stats..\n");
	printf("Largest value found across files: %f. Smallest value found: %f\n", globalMax, globalMin);

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
