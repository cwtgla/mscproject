#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>

struct runlengthEntry { //struct to represent a runlength entry of value and number of times its repeated
	float value;
	uint32_t valuecount;
};

struct compressedVal { //struct to represent a multiple*8 bit compressed value
	unsigned char *data;
};

/* 
 * Purpose:  
 * 		Get a list of the absolute filepaths of a certain type of file within a directory
 * Parameters: 
 * 		1. files[] - a blank char array to hold the absolute file paths desired
 * 		2. baseDirectory - the absolute path of the base directory that the target files are in
 * 		3. fileExtension - the filename extension of the targeted files in the base directory
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
 * Purpose:
 * 		Decompress a runlength compressed set of values into a list of floats.
 * Returns:
 * 		List of floats represented the uncompressed version of the given data
 * Parameters:
 * 		1. compressedValues - a list of runlength compress runlengthEntry structs
 * 		2. count - a count of the number of (unique) values in the runlength compressed data
 * 		3. newCount - a count of the number of entries in the decompressed data
 */
float *runlengthDecompression(struct runlengthEntry *compressedValues, int count, int *newCount) {
	int i;
	unsigned int totalCount = 0;

	for(i = 0; i < count; i++) {
		totalCount+= compressedValues[i].valuecount;
	}
	float *uncompressedValues = malloc(totalCount*sizeof(float));
	
	int newPos = 0;
	for(i = 0; i < count; i++) {
		while(compressedValues[i].valuecount != 0) {
			uncompressedValues[newPos] = compressedValues[i].value;
			newPos++;
			compressedValues[i].valuecount--;
		}
	}
	*newCount = newPos;
	return uncompressedValues;
}

/* 
 * Purpose:
 *		Using runlength compression, get a compressed version of a given dataset of floats
 * Returns:
 * 		list of rlEntrys that represent a runlength compressed version of *values
 * Parameters:
 * 		1. values - list of float values that are to be compressed
 * 		2. count - a count of the number of values in the given data
 * 		3. newCount - a count of the number of entries in the runlength compressed data
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
	ci++;
	*newCount =  ci;
	return compressedData;
}

/*
 * Purpose:
 * 		Get the float data contained in a given file
 * Returns:
 * 		list of floats representing data in the file
 * Parameters:
 * 		1. absFilePath - absolute file path of the file that data will be extracted from
 * 		2. dataLength - passed in to get the number of values/indexes of the data
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

/*
 * Purpose:
 * 		Compress the given data into a 24 bit format using the given parameters to cut down the original data
 * Returns:
 * 		List of 24 bit values representing the original 32bit data
 * Parameters:
 * 		1. absFilePath - absolute file path of the file that data will be extracted from
 * 		2. magBits - number of bits to be used for the magnitude of the data (before decimal place)
 *		3. precBits - number of bits to be used for the precision of the data (after decimal place)
 */
struct compressedVal *get24BitCompressedData(char *absFilePath, unsigned int magBits, unsigned int precBits) {
	int dataLength = 0;
	float *uncompressedData = getData(absFilePath, &dataLength);
	struct compressedVal *compressedData = calloc(dataLength, sizeof(struct compressedVal));
	char sign;
	float firstPart, secondPart;
	uint32_t beforeDecimal, afterDecimal;
	int i;
	int offset;
	void *temp;
	char *bytes;

	for(i = 0; i < dataLength; i++) {
		compressedData[i].data = malloc(3 * sizeof(char));
		sign = uncompressedData[i]>0?0:1;
		secondPart = modff(uncompressedData[i], &firstPart);
		beforeDecimal = (uint32_t) firstPart;
		afterDecimal = (uint32_t) (secondPart*10000);
		compressedData[i].data[0] = sign << 7;
		temp = &beforeDecimal;
		bytes = (char *) (temp);
		printf("asd %d %d\n", bytes[2], bytes[2] << 2);
		compressedData[i].data[0] = compressedData[i].data[0] | bytes[0] << 2;
		temp = &afterDecimal;
		bytes = (char *) (temp);
		compressedData[i].data[0] = compressedData[i].data[0] | bytes[3]; 
		compressedData[i].data[1] = bytes[1];
		compressedData[i].data[2] = bytes[0];
		printf("start %d %d\n", beforeDecimal, afterDecimal);
		printf("after %d:%d:%d\n\n", compressedData[i].data[0], compressedData[i].data[1], compressedData[i].data[2]);
	}
	free(uncompressedData);
	return compressedData;
}