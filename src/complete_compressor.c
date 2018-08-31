//FILE: complete_compressor.c 
//AUTHOR: Craig Thomson
//PURPOSE: contains all functions used for compression/decompression and other utilities

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <math.h>
#include <assert.h>
#include "complete_compressor.h"
#include <float.h>

struct floatSplitValue { //Struct to represent the before and after decimal point values of a float split (3 bytes for each)
	uint8_t *beforeDecimal;
	uint8_t *afterDecimal;
};

/* 
 * Purpose:  
 * 		Get a array of the absolute filepaths of a certain type of file within a directory.
 * Parameters: 
 * 		1. files - A blank char array to hold the absolute file paths desired.
 * 		2. baseDirectory - The absolute path of the base directory that the target files are in.
 * 		3. fileExtension - The filename extension of the targeted files in the base directory.
 *		4. count - A blank pointer which later getss set to the number of files found.
 */
void getAbsoluteFilepaths(char *files[], char *baseDirectory, char *fileExtension, int *count) {
	struct dirent *directoryEntry;
	DIR *directory = opendir(baseDirectory);
	int pathLen = strlen(baseDirectory)+1; //For null char
	int i = 0;
	
	if(directory != NULL) {
		while((directoryEntry = readdir(directory)) != NULL) { //Whilst there's still entries
			if(strstr(directoryEntry->d_name, fileExtension) != NULL) { //If the entry is of the desired extension
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
 * 		Extract a list of floats from a given file. This method is used to get the simulation data from data dump files. (/simulation datasets/)
 * Returns:
 * 		An array of floats representing data in the file.
 * Parameters:
 * 		1. absFilePath - Absolute file path of the file that data will be extracted from.
 * 		2. count - Blank pointer passed in to be assigned to the number of indexes in the returned array.
 *		3. max - Blank pointer passed in to be assigned to the maximum value in the returned array.
 *		4. min - Blank pointer passed in to be assigned to the minimum value in the returned array.
 *		5. mean - Blank pointer passed in to be assigned to the average value of the returned array.
 */
float *getData(char *absFilePath, int *count, float *max, float *min, float *mean) {
	FILE *contentFile = fopen(absFilePath, "r");
	float *fileContent = malloc((150*150*150)*sizeof(float)); //arbitrarily large allocation for other datasets
	int i = 0;
	*max = FLT_MIN;
	*min = FLT_MAX;
	float total = 0;

	while(fscanf(contentFile, "%f", &fileContent[i]) == 1) { //while there's still data left, copy it into the array
		total+=fileContent[i];
		if(fileContent[i] > *max) {
			*max = fileContent[i];
		} else if(fileContent[i] < *min) {
			*min = fileContent[i];
		}
		i++;
	}
	fclose(contentFile);
	float *exactContent = malloc(i*sizeof(float)); //resize to whats needed
	memcpy(exactContent, fileContent, i*sizeof(float));
	*count = i;
	*mean = total/ *count;

	return exactContent;
}

/*
 * Purpose:
 * 		Extract a list of ints from a given file. This method is used to get ints representing values of bytes which is used for testing purposes.
 * Returns:
 * 		An array of ints representing data in the file
 * Parameters:
 * 		1. absFilePath - absolute file path of the file that data will be extracted from
 * 		2. dataLength - passed in to get the number of values/indexes of the data
 */
int *getVerificationData(char *absFilePath, int *dataLength) {
	FILE *contentFile = fopen(absFilePath, "r");
	int *fileContent = malloc(100*sizeof(int)); //arbitrarily large allocation (supports bigger test files) 
	int i = 0;

	while(fscanf(contentFile, "%d", &fileContent[i]) == 1) { //while there's still data left, copy it into the array
		i++;
	}
	fclose(contentFile);
	int *exactContent = malloc(i*sizeof(int));
	memcpy(exactContent, fileContent, i*sizeof(int)); //resize to whats needed
	*dataLength = i;

	return exactContent;
}

/*
 * Purpose:
 *		Get the number of digits required to represent an numBits bits number.
 * Returns:
 *		The maximum number of digits needed to represent a numBits size number. (e.g. pass numBits=7, 2^7=128, 3 digits).
 * Parameters:
 *		1. numBits - The number of bits the number needs to represent it.
 */
int numDigits (int numBits) {
    if (numBits == 0) return 1;
    return floor (log10 (pow(2,numBits))) + 1;
}

/*
 * Purpose:
 * 		Split a float into 2 ints (used for 24 bit compression, turning the float into 2 ints before storing).
 * Returns:
 * 		A floatSplitValue struct which represents the passed float.
 * Parameters:
 * 		1. value - The initial float value to be broken up.
 * 		2. multiplier - How much to multiply the value after the decimal point to. (10,100,10,000 ...).
 */
struct floatSplitValue splitFloat(float value, unsigned int multiplier) {
	float beforeDp, afterDp;
	uint32_t before, after;

	afterDp = modff(value, &beforeDp); //modff to get the before and after decimal as floats
	before = (uint32_t) fabs(beforeDp);
	after = (uint32_t) (fabs(afterDp) * multiplier);

	uint8_t *beforeBytes = malloc(3*sizeof(uint8_t)); //strip out byte by byte
	beforeBytes[2] = (before >> 16) & 0xFF;
	beforeBytes[1] = (before >> 8) & 0xFF;
	beforeBytes[0] = before & 0xFF;

	uint8_t *afterBytes = malloc(3*sizeof(uint8_t));
	afterBytes[2] = (after >> 16) & 0xFF;
	afterBytes[1] = (after >> 8) & 0xFF;
	afterBytes[0] = after & 0xFF;

	struct floatSplitValue split = { .beforeDecimal = beforeBytes, .afterDecimal = afterBytes};

	return split;
}

/* 
 * Purpose:
 *		Using runlength compression, compress an array of floats.
 * Returns:
 * 		Array of runlengthEntrys that represent a runlength compressed version of the given values.
 * Parameters:
 * 		1. values - Array of float values that are to be compressed.
 * 		2. count - A count of the number of values in the given data.
 * 		3. newCount - A count of the number of entries in the runlength compressed data.
 */
struct runlengthEntry *getRunlengthCompressedData(float *values, int count, int *newCount) {
	struct runlengthEntry *compressedData = calloc(count, sizeof(struct runlengthEntry));
	compressedData[0].value = values[0]; //Have to initilise the first index then work from there
	compressedData[0].valueCount = 1;
	int uci, ci = 0; //ci = compressed ds index, uci = uncompressed ds index

	for(uci=1; uci < count; uci++) {
		if(compressedData[ci].value == values[uci]) { //If value is continued
			compressedData[ci].valueCount++;
		} else { //New value, create an entry to store it
			ci++;
			compressedData[ci].value = values[uci];
			compressedData[ci].valueCount = 1;
		}
	}
	*newCount =  ci+1;
	return compressedData;
}

/*
 * Purpose:
 * 		Decompress a runlength compressed set of floats into the original array of floats.
 * Returns:
 * 		Array of floats represented the uncompressed version of the given data.
 * Parameters:
 * 		1. compressedValues - a array of runlengthEntry structs representing a lsit of runlength compressed floats
 * 		2. count - the number of runlengthEntry structs that compressedValues contains
 * 		3. newCount - blank pointer that gets assigned the number of entries in the returned decompressed data
 */
float *getRunlengthDecompressedData(struct runlengthEntry *compressedValues, int count, int *newCount) {
	int i;
	int totalCount = 0;

	for(i = 0; i < count; i++) { //find out the number of elements (if decompressed) compressedValues contains
		totalCount+= compressedValues[i].valueCount;
	}
	float *uncompressedValues = malloc(totalCount*sizeof(float));
	
	int newPos = 0;
	for(i = 0; i < count; i++) {
		while(compressedValues[i].valueCount != 0) { //"unwrap" the current entry and create elements for it
			uncompressedValues[newPos] = compressedValues[i].value;
			newPos++;
			compressedValues[i].valueCount--;
		}
	}
	*newCount = newPos;
	return uncompressedValues;
}

/*
 * Purpose:
 * 		Compress the given data into a 24 bit format using the given parameters to cut down the original data.
 * Returns:
 * 		Array of compressedVal (24 bit) values representing a compressed version the original array of 32 bit floats.
 * Parameters:
 * 		1. uncompressedData - List of 32 bit floats to be compressed
 *		2. count - The number of values in the parameter 1.
 * 		2. magBits - Number of bits to be used to represent the magnitude of the data (bits used for before decimal place).
 *		3. precBits - Number of bits to be used to represent the precision of the data (bits used for after decimal place).
 */
struct compressedVal *get24BitCompressedData(float *uncompressedData, unsigned int count, unsigned int magBits, unsigned int precBits) {
	struct compressedVal *compressedData = calloc(count, sizeof(struct compressedVal)); //Create array for new compressed values
	unsigned int multiplier;
	if(numDigits(precBits) == 1) {
		multiplier = 10;
	} else {
		multiplier = pow(10, numDigits(precBits)-1); //max number of digits that can be represented by a precBits number
	}
	
	unsigned int i, space, target, ci, uci; //ci = compressed index, uci = uncompressed indexspace is byte space, target is number of bits trying to move
	struct floatSplitValue value;

	for(i = 0; i < count; i++) {
		value = splitFloat(uncompressedData[i], multiplier); //turn float into 2 ints representing before and after decimal

		if (uncompressedData[i] < 0) { //Set the sign bit (1 for -ve, 0 for +ve)
			compressedData[i].data[2] = 1 << 7;
		}
		if(magBits + 1 == 8) { //Case with 7 bits of magnitude, 16 of precision
			compressedData[i].data[2] = compressedData[i].data[2] | value.beforeDecimal[0];
			compressedData[i].data[1] = value.afterDecimal[1];
			compressedData[i].data[0] = value.afterDecimal[0];
		} 
		else if(magBits + 1 < 8) { //Case where some precision bits are in byte 2 (small mag, high prec)
			compressedData[i].data[2] = compressedData[i].data[2] | (value.beforeDecimal[0] << (7-magBits));
			compressedData[i].data[2] = compressedData[i].data[2] | value.afterDecimal[2];
			compressedData[i].data[1] = value.afterDecimal[1];
			compressedData[i].data[0] = value.afterDecimal[0];
		} else if(magBits + 1 > 8) { //Case where magnitude takes up 8 or more bits
			space = 7; //8-sign
			ci = 2; //start on LHS byte of compressed struct

			if(magBits >= 17) { //magnitude all 3 bytes
				uci = 2;
				target = magBits-16;
			} else if(magBits >= 9) { // magnitude in byte 2
				uci = 1;
				target = magBits - 8;
			} else { //magnitude in byte 1
				uci = 0;
				target = magBits;
			}

			while (target != 0 && uci >= 0) {	//While we have bits to insert and we've got bytes to extract from
				if (space >= target) { //If we can fit the current values byte into the compressed byte

					compressedData[i].data[ci] = compressedData[i].data[ci] | (value.beforeDecimal[uci] << (space-target));
					space = space - target;
					if(space == 0) { //If there's no space left, move to the next compressed byte
						ci--;
						space = 8;
					}
					if(uci > 0) { //If we're not on our last byte, decrement
						uci--;
						target = 8;
					} else { //Done, get out of while
						uci--;
						target = 0;
					}
				} else { //We're trying to squeeze more data than we have free indexes
					compressedData[i].data[ci] = compressedData[i].data[ci] | (value.beforeDecimal[uci] >> (target-space));
					ci--;
					target = target-space;
					space = 8;
				}
			}
			if(magBits >= 17) { //Extract the magnitude, either in last byte or part of byte 1 and all of last byte
				compressedData[i].data[0] = compressedData[i].data[0] | (value.afterDecimal[0] & (((uint32_t) pow(2, precBits)) - 1));
			} else {
				compressedData[i].data[1] = compressedData[i].data[1] | value.afterDecimal[1];
				compressedData[i].data[0] = value.afterDecimal[0];
			}
		}
	}
	return compressedData;
}

/*
 * Purpose:
 * 		Decompress the 24 bit format data into a version of the original data with some precision lost, depending on magnitude and precision sizes.
 * Returns:
 * 		Array of floats representing the data contained in the 24 bit format.
 * Parameters:
 * 		1. values - An array of 24 bit compressed values.
 *		2. count - The number of 24 bit compressed values in parameter 1.
 * 		2. magBits - Number of bits that have been used to represent magnitude in the 24 bit notation.
 *		3. precBits - Number of bits that have been used to represent precision in the 24 bit notation.
 */
float *get24BitDecompressedData(struct compressedVal *values, unsigned int count, unsigned int magBits, unsigned int precBits) {
	float *uncompressed = calloc(count, sizeof(float));
	unsigned int divider;
	if(numDigits(precBits) == 1) {
		divider = 10;
	} else {
		divider = pow(10, numDigits(precBits)-1);
	}
	unsigned int beforeDp = 0;
	unsigned int afterDp = 0;
	int i, signMultiplier;

	for(i = 0; i < count; i++) {
		beforeDp = 0;
		afterDp = 0;

		if((values[i].data[2] >> 7) == 1) { //if sign bit is set then it's a -ve number represented 
			signMultiplier = -1;
		} else {
			signMultiplier = 1;
		}
		if(magBits + 1 < 8) { //Magnitude in byte 2 as well as precision and precision in byte 1 and 0
			beforeDp = (values[i].data[2] >> (7-magBits)) & (uint32_t) (pow(2, magBits)-1); //Shift off the extra precision then AND to keep bits of interest
			afterDp = (values[i].data[2] & (uint32_t) (pow(2,(precBits % 8))-1)) << 16; //use AND to keep the number of bits on RHS we want for precision and shift into place
			afterDp = afterDp | (values[i].data[1] << 8);
			afterDp = afterDp | values[i].data[0];
		} else if(magBits + 1 == 8) { //Magnitude and sign bit fill up byte 2, precision in other 2 bytes
			beforeDp = values[i].data[2] & (uint32_t) (pow(2, magBits)-1); //Extract RHS 7 bits we want and take precision
			afterDp = values[i].data[1] << 8;
			afterDp = afterDp | values[i].data[0];
		} else if(magBits+1 > 8) { //Magnitude fills over a byte and more
			beforeDp = (values[i].data[2] & (((uint32_t) pow(2,7)) - 1)) << (magBits - 7); //Extract last 7 bits from the first byte
			int magBitsLeft = magBits - 7;
			int precBitsLeft = precBits;
			int ci = 1; //compressed data index

			while(magBitsLeft != 0) {
				if(magBitsLeft >= 8) { //Whole byte or more of interest
					beforeDp = beforeDp | (values[i].data[ci] << (magBitsLeft-8));
					magBitsLeft = magBitsLeft - 8;
					ci--;
				} else {
					beforeDp = beforeDp | ((((uint32_t) pow(2,8)-1) - ((uint32_t) pow(2,8-magBitsLeft)-1)) & values[i].data[ci]) >> (8-magBitsLeft);
					magBitsLeft = 0;
				}
			}


			while(precBitsLeft != 0) {
				if(precBitsLeft >= 8) { //Precision is the whole byte
					printf("val is %u\n", values[i].data[ci]);
					afterDp = afterDp | values[i].data[ci];
					printf("val2 is %u\n", afterDp);
					precBitsLeft = precBitsLeft - 8;
				} else { //Precision is on RHS and part on LHS is to be ignored
					afterDp = ((uint32_t) pow(2, precBits)-1) & values[i].data[ci];// << (precBitsLeft - (precBitsLeft%8));
					//printf("before %u after %u dividor is %d res is %f %f\n",beforeDp, afterDp, divider, (float) afterDp/divider);
					ci--;
					precBitsLeft = 0;
				}
			}
		}

		//float part1 = afterDp / (float) divider;
		//float part2 = part1 + (float) beforeDp;
	//	printf("%.10f %.10f %.10f %.10f\n\n", part1, (float) beforeDp, part2, ((float) beforeDp) + part1);
		//uncompressed[i] = signMultiplier * part2;
		//float afterVal = ((float) afterDp) / divider;
		//uncompressed[i] = signMultiplier * (beforeDp + afterVal);
		//printf("just before %f\n", (beforeDp + ((float) afterDp) / divider));
		printf("div is %d before is %u after is %u", divider, beforeDp, afterDp);
		uncompressed[i] = signMultiplier * (beforeDp + ((float) afterDp) / divider);
	}
	return uncompressed;
}