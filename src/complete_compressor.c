#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <math.h>
#include "complete_compressor.h"

struct runlengthEntry { //struct to represent a runlength entry of value and number of times its repeated
	float value;
	uint32_t valuecount;
};

/*
 * Purpose:
 *		Get the number of digits required to represent an numBits size number.
 * Returns:
 *		The maximum number of digits needed to represent a numBits number. (e.g. 2^7=128, 3 digits)
 * Parameters:
 *		1. numBits - the number of bits the number needs to represent it
 */
int numDigits (int numBits) {
    if (numBits == 0) return 1;
    return floor (log10 (pow(2,numBits))) + 1;
}

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
		while(compressedValues[i].valuecount != 0) { //whilst there's still entries to unpack
			uncompressedValues[newPos] = compressedValues[i].value;
			newPos++;
			compressedValues[i].valuecount--;
		}
	}
	//newPos++;
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
	*newCount =  ci+1;
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

int *getVerificationData(char *absFilePath, int *dataLength) {
	FILE *contentFile = fopen(absFilePath, "r");
	int *fileContent = malloc(100*sizeof(int)); //arbitrarily large allocation 
	int i = 0;

	while(fscanf(contentFile, "%d", &fileContent[i]) == 1) { //while there's still data left, copy it into the array
		i++;
	}
	fclose(contentFile);
	int *exactContent = malloc(i*sizeof(int));
	memcpy(exactContent, fileContent, i*sizeof(int)); //cut down to what you need
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
	struct compressedVal *compressedData = calloc(dataLength, sizeof(struct compressedVal)); //get ds for new compressed data
	char signBit;
	unsigned int multiplier = pow(10, numDigits(precBits)-1); //max number of digits that can be represented by a precBits number
	printf("mul %u\n", multiplier);
	float beforeDp, afterDp;
	uint32_t before, after;
	void *tmp;
	char *uncompressedBytes;

	int i;

	for(i = 0; i < dataLength; i++) {
		//Initial setup and extract values to be stored
		//compressedData[i].data = malloc(3 * sizeof(char)); //24 numBitsLOOK 
		afterDp = modff(uncompressedData[i], &beforeDp); //extract values before and after decimal
		//printf("1. %f\n", beforeDp);
		//printf("2. %u\n", (uint32_t) abs(beforeDp));
		before = (uint32_t) abs(beforeDp);
		//printf("before decimal. %u\n", before);
		//printf("before format after decimal %f\n", afterDp);
		//printf("mult %f\n", afterDp*multiplier);
		//printf("abs %f %d\n", abs(afterDp), abs(afterDp*multiplier));
		after = (uint32_t) (fabs(afterDp) * multiplier);
		printf("after decimal. %u\n", after);
		//Extract sign bit and shift in
		signBit = uncompressedData[i] > 0 ? 0 : 1; //1 for -ve 0 for +ve
		//printf("sign bit %d\n", signBit);
		compressedData[i].data[0] = signBit << 7;

		//Extract before decimals bytes and shift in
		tmp = &before;
		uncompressedBytes = (char *) (tmp);
		//printf("values before OR %u %u %u\n", compressedData[i].data[0], uncompressedBytes[0] << (7-magBits), compressedData[i].data[0] | uncompressedBytes[0] << (7-magBits));
		compressedData[i].data[0] = compressedData[i].data[0] | (uncompressedBytes[0] << (7-magBits));
		//Extract after decimals bytes and shift in
		tmp = &after;
		uncompressedBytes = (char *) (tmp);
		compressedData[i].data[0] = compressedData[i].data[0] | uncompressedBytes[2];
		compressedData[i].data[1] = uncompressedBytes[1];
		compressedData[i].data[2] = uncompressedBytes[0];
		//printf("before break %f\n", uncompressedData[i]);
		//printf("after %u:%u:%u\n\n", compressedData[i].data[0], compressedData[i].data[1], compressedData[i].data[2]);
	}
	
	//printf("split %u %u\n", before, after);
	

	free(uncompressedData);
	return compressedData;
//	int dataLength = 0;


	//int offset;
	//void *temp;
	//char *bytes;

	//for(i = 0; i < dataLength; i++) {
		//compressedData[i].data = malloc(3 * sizeof(char));
		//sign = uncompressedData[i]>0?0:1;
		//secondPart = modff(uncompressedData[i], &firstPart);
		//beforeDecimal = (uint32_t) firstPart;
		//afterDecimal = (uint32_t) (secondPart*(pow(10,precBits)));

		//compressedData[i].data[0] = sign << 7;
		//temp = &beforeDecimal;
		//b//ytes = (char *) (temp);
		//printf("asd %d %d\n", bytes[2], bytes[2] << 2);
		//compressedData[i].data[0] = compressedData[i].data[0] | bytes[0] << 2;
		//temp = &afterDecimal;
		//bytes = (char *) (temp);
		//compressedData[i].data[0] = compressedData[i].data[0] | bytes[3]; 
		//compressedData[i].data[1] = bytes[1];
		//compressedData[i].data[2] = bytes[0];

	//}
//	free(uncompressedData);
//	return compressedData;
}