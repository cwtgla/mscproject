#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

struct runlengthEntry {
	float value;
	uint valuecount;
};

/* 
 * Purpose:
 * 		Get a list of the absolute filepaths of a certain type of file within a directory
 * Parameters: 
 * 		1. *files[] - a blank char array to hold the absolute file paths desired
 * 		2. *baseDirectory - the absolute path of the base directory that the target files are in
 * 		3. *fileExtension - the filename extension of the targeted files in the base directory
 */
void getAbsoluteFilepaths(char *files[], char *baseDirectory, char *fileExtension, int *count) {
	struct dirent *directoryEntry;
	DIR *directory = opendir(baseDirectory);
	int pathLen = strlen(baseDirectory)+1; //for null char
	int i = 0;
	
	if(directory != NULL) {
		//whilst there's still entries
		while((directoryEntry = readdir(directory)) != NULL) {
			//if the entry is of the desired extension
			if(strstr(directoryEntry->d_name, fileExtension) != NULL) {
				files[i] = calloc((strlen(directoryEntry->d_name) + pathLen), sizeof(char));
				strcat(files[i], baseDirectory);
				strcat(files[i], directoryEntry->d_name);
				i++;
			}
		}
		*count = i;
	} 
}


/* 
 *	Purpose:
 *		Using runlength compression, get a compressed version of a given dataset of floats
 * Returns:
 * 		list of rlEntrys that represent a runlength compressed version of *values
 * Parameters:
 * 		1. *values - list of float values that are to be compressed
 * 		2. *count - a count of the number of values in the given data
 * 		3. *newCount - a count of the number of entries in the runlength compressed data
 */
struct runlengthEntry *runlengthCompression(float *values, int count, int *newCount) {
	struct runlengthEntry *compressedData = calloc(count, sizeof(struct runlengthEntry));
	compressedData[0].value = values[0];
	compressedData[0].valuecount = 1;
	int uci, ci = 0; //ci = compressed ds index, uci = uncompressed ds index

	for(uci=1; uci < count; uci++) {
		if(compressedData[ci].value == values[uci]) { //if value is continued
			compressedData[ci].valuecount++;
		} else {
			ci++;
			compressedData[ci].value = values[uci];
			compressedData[ci].valuecount = 1;
		}
	}
	*newCount = ci;
	return compressedData;
}

/*
 * Purpose:
 * 		Get the float data contained in a given file
 * Returns:
 * 		list of floats representing data in the file
 * Parameters:
 * 		1. *absFilePath - absolute file path of the file that data will be extracted from
 * 		2. *dataLength - passed in to get the number of values/indexes of the data
 *
 */
float *getData(char *absFilePath, int *dataLength) {
	FILE *contentFile = fopen(absFilePath, "r");
	float *fileContent = malloc((150*150*150)*sizeof(float)); //arbitrarily large allocation for other datasets
	int i = 0;

	while(fscanf(contentFile, "%f", &fileContent[i]) == 1) { //while there's still data left, copy it into the array
		i++;
	}
	fclose(contentFile);
	float *exactContent = malloc(i*sizeof(float));
	memcpy(exactContent, fileContent, i*sizeof(float)); //cut down to what you need
	*dataLength = i;

	return exactContent;
}

//Takes in option and path. analysis or compress and a path to either a folder or a file
int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Need at least 1 argument (base directory path)\n"); 
		return -1;
	}
	//Create base array for filepaths
	char *filePaths[100];
	int count;
	getAbsoluteFilepaths(filePaths, argv[1], ".txt.clean", &count);

	int i;
	printf("%d\n", count);
	for(i=0; i<count; i++) {
		printf("%d %s\n", i, filePaths[i]);
	}
	
	///get uncompressed data
	int uncompressedSize = 0;
	printf("passing %s\n", filePaths[0]);
	float *uncompressedData = getData(filePaths[0], &uncompressedSize);
	//for(i = 0; i < uncompressedSize; i++) {
	//	printf("%d %.8f\n", i, uncompressedData[i]);
	//}


	int compressedSize;
	//printf("%d", uncompressedSize);
	struct runlengthEntry *compressedData = runlengthCompression(uncompressedData, uncompressedSize, &compressedSize);
	for(i=0; i<compressedSize;i++) {
		printf("index: %d value: %f count: %d\n", i, compressedData[i].value, compressedData[i].valuecount);
		//if(compressedData[i].valuecount > 1) {
		//	printf("index: %d value: %f count: %d\n", i, compressedData[i].value, compressedData[i].valuecount);
		//}
	}

}