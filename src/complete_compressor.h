//FILE: complete_compresor.h 
//AUTHOR: Craig Thomson
//PURPOSE: headers for functions used for compression/decompression/etc by test/data analysis files
#include <stdint.h>

#ifndef COMPLETE_COMPRESSOR_H_
#define COMPLETE_COMPRESSOR_H_

struct compressedVal { //struct to represent a multiple*8 bit compressed value
	unsigned char data[3];
};

struct runlengthEntry { //Struct to represent a runlength entry of value and number of times its repeated
	float value;
	uint32_t valueCount;
};

void getAbsoluteFilepaths(char *files[], char *baseDirectory, char *fileExtension, int *count);

float *getData(char *absFilePath, int *count, float *max, float *min, float *mean);

int *getVerificationData(char *absFilePath, int *dataLength);

int numDigits (int numBits);

struct runlengthEntry *getRunlengthCompressedData(float *values, int count, int *newCount);

float *getRunlengthDecompressedData(struct runlengthEntry *compressedValues, int count, int *newCount);

struct compressedVal *get24BitCompressedData(float *uncompressedData, unsigned int count, unsigned int magBits, unsigned int precBits);

float *get24BitDecompressedData(struct compressedVal *values, unsigned int count, unsigned int magBits, unsigned int precBits);

unsigned char *getVariableBitCompressedData(float *uncompressedData, unsigned int count, unsigned int *newCount, unsigned int magBits, unsigned int precBits);

float *getVariableBitDecompressedData(unsigned char *values, unsigned int count, unsigned int *newCount, unsigned int magBits, unsigned int precBits);

#endif