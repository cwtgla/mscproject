//FILE: compressor.h 
//AUTHOR: Craig Thomson
//PURPOSE: headers for functions used for compression/decompression/etc by test/data analysis files
#include <stdint.h>

#ifndef COMPRESSOR_H_
#define COMPRESSOR_H_

struct compressedVal { //struct to represent a multiple*8 bit compressed value
	unsigned char data[3];
};

struct runlengthEntry { //Struct to represent a runlength entry of value and number of times its repeated
	float value;
	uint32_t valueCount;
};

void getAbsoluteFilepaths(char *files[], char *baseDirectory, char *fileExtension, unsigned int *count);

float *getData(char *absFilePath, unsigned int *count, float *max, float *min, float *mean);

unsigned int *getVerificationData(char *absFilePath, unsigned int *dataLength);

unsigned int numDigits (unsigned int numBits);

struct runlengthEntry *getRunlengthCompressedData(float *values, unsigned int count, unsigned int *newCount);

float *getRunlengthDecompressedData(struct runlengthEntry *compressedValues, unsigned int count, unsigned int *newCount);

struct compressedVal *get24BitCompressedData(float *uncompressedData, unsigned int count, unsigned int magBits, unsigned int precBits);

float *get24BitDecompressedData(struct compressedVal *values, unsigned int count, unsigned int magBits, unsigned int precBits);

float get24BitCompressedValue(struct compressedVal *allValues, unsigned int index, unsigned int magBits, unsigned int precBits);

void storeValueAs24Bit(struct compressedVal *allValues, float updatedValue, unsigned int index, unsigned int magBits, unsigned int precBits);

unsigned char *getVariableBitCompressedData(float *uncompressedData, unsigned int count, unsigned int *newCount, unsigned int magBits, unsigned int precBits);

float *getVariableBitDecompressedData(unsigned char *values, unsigned int count, unsigned int *newCount, unsigned int magBits, unsigned int precBits);

float getVariableBitDecompressedValue(unsigned char *values, unsigned int count, unsigned int targetIndex, unsigned int magBits, unsigned int precBits);

void insertVariableBitValue (unsigned char *values, unsigned int count, unsigned int targetIndex, float value, unsigned int magBits, unsigned int precBits);
#endif