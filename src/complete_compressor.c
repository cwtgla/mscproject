#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <math.h>
#include <assert.h>
#include "complete_compressor.h"

struct runlengthEntry { //struct to represent a runlength entry of value and number of times its repeated
	float value;
	uint32_t valuecount;
};

struct brokenValue { //struct to represent a float broken up into 2 ints, each pointer has 4 objects (32 bytes)
	uint8_t *beforeDecimal;
	uint8_t *afterDecimal;
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

/*
 * Purpose:
 * 		Get the int data contained in a given file
 * Returns:
 * 		list of int representing unsigned bytes in decimal
 * Parameters:
 * 		1. absFilePath - absolute file path of the file that data will be extracted from
 * 		2. dataLength - passed in to get the number of values/indexes of the data
 */
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
 * 		Break a float into 2 ints and get it back
 * Returns:
 * 		a brokenValue struct which represents the float
 * Parameters:
 * 		1. value - The initial float value to be broken up
 * 		2. multiplier - what order of 10 to multiply the value after the decimal point to
 */
struct brokenValue breakFloat(float value, unsigned int multiplier) {
	float beforeDp, afterDp;
	uint32_t before, after;
	void *tmp1, *tmp2;
	afterDp = modff(value, &beforeDp); //split the initial float into 2 floats, one for before decimal and the other for after
	before = (uint32_t) abs(beforeDp);
	after = (uint32_t) (fabs(afterDp) * multiplier);
	tmp1 = &before;
	tmp2 = &after;
	struct brokenValue split = { .beforeDecimal =  (char *) (tmp1), .afterDecimal = (char *) (tmp2)}; 

	return split;
}

/*
 * Purpose:
 * 		Deompress the 24 bit format data into a version of the original data with some precision lost
 * Returns:
 * 		List of 32bit ints representing the data contained in the 24 bit format
 * Parameters:
 * 		1. values - the 24 bit values to be decompressed
 * 		2. magBits - number of bits used to represent magnitude in the 24 bit notation
 *		3. precBits - number of bits used to represent precision in the 24 bit notation
 */
int *get24BitDecompression(struct compressedVal *values, unsigned int magBits, unsigned int precBits) {
	return NULL;

	//sign = byte 2 shift right 7, if its 1 then -ve else positive

	//if mag+1 == 8

	//if mag+1 less than 8

	//if mag+1 more than 8
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
	unsigned int multiplier = pow(10, numDigits(precBits)-1); //max number of digits that can be represented by a precBits number
	unsigned int i, space, target, compInd, valueInd;

	assert (magBits + precBits + 1 == 24);

	//Given a 32 bit number fit it into 24
	for(i = 0; i < dataLength; i++) {
		//break float into 2 ints representing before and after decimal values
		struct brokenValue value = breakFloat(uncompressedData[i], multiplier);

		//If it's negative then shift a 1 in, else 0 and do nothing 
		if (uncompressedData[i] < 0) {
			compressedData[i].data[2] = 1 << 7;
		}
		if(magBits + 1 == 8) { //Case with 7 bits of magnitude, 16 of precision
			compressedData[i].data[2] = compressedData[i].data[2] | value.beforeDecimal[0];
			compressedData[i].data[1] = value.afterDecimal[1];
			compressedData[i].data[0] = value.afterDecimal[0];
		} 
		else if(magBits + 1 < 8) { //Case where some precision bits go into byte 2 (small mag, high prec)
			compressedData[i].data[2] = compressedData[i].data[2] | (value.beforeDecimal[0] << (7-magBits));
			compressedData[i].data[2] = compressedData[i].data[2] | value.afterDecimal[2];
			compressedData[i].data[1] = value.afterDecimal[1];
			compressedData[i].data[0] = value.afterDecimal[0];
		} else if(magBits + 1 > 8) { //Case where magnitude takes up 8 or more bits
			space = 7; //8-sign
			compInd = 2; //start on LHS byte of compressed struct

			if(magBits >= 17) {
				valueInd = 2;
				target = magBits-16;
			} else if(magBits >= 9) {
				valueInd = 1;
				target = magBits - 8;
			} else {
				valueInd = 0;
				target = magBits;
			}
			//While we have bits to insert or we're not on the last byte to insert
			while (target != 0 || valueInd != 0) {
				if (space >= target) { //If we can fit the current values byte into the compressed byte
					compressedData[i].data[compInd] = compressedData[i].data[compInd] || value.beforeDecimal[valueInd] << (space-target);
					space = space - target;
					if(space == 0) { //If there's no space left, move to the next compressed byte
						compInd--;
						space = 8;
					}
					//Since we've emptied the current values bytes, move onto the next one
					if(valueInd > 0) { //If we're not on our last byte, decrement
						valueInd--;
						target = 8;
					} else { //We're finished
						valueInd--;
						target = 0;
					}
				} else { //We're trying to squeeze more data than we have free indexes
					compressedData[i].data[compInd] = compressedData[i].data[compInd] || value.beforeDecimal[valueInd] >> (target-space);
					compInd--;
					target = target-space;
					space = 8;
				}
			}
			//Push in the magnitude, either in last byte or last byte and part of the 2nd last
			if(magBits >= 17) { 
				compressedData[i].data[0] = compressedData[i].data[0] || value.afterDecimal[0];
			} else {
				compressedData[i].data[1] = compressedData[i].data[1] || value.afterDecimal[1];
				compressedData[i].data[0] = value.afterDecimal[0];
			}
		}
	}
	free(uncompressedData);
	return compressedData;
}