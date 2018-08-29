//FILE: complete_compresor.h 
//AUTHOR: Craig Thomson
//PURPOSE: headers for functions used for compression/decompression/etc by test/data analysis files
#ifndef COMPLETE_COMPRESSOR_H_
#define COMPLETE_COMPRESSOR_H_

struct compressedVal { //struct to represent a multiple*8 bit compressed value
	unsigned char data[3];
};

void getAbsoluteFilepaths(char *files[], char *baseDirectory, char *fileExtension, int *count);

float *getData(char *absFilePath, int *dataLength);

int *getVerificationData(char *absFilePath, int *dataLength);

int numDigits (int numBits);

struct runlengthEntry *getRunlengthCompressedData(float *values, int count, int *newCount);

float *getRunlengthDecompressedData(struct runlengthEntry *compressedValues, int count, int *newCount);

struct compressedVal *get24BitCompressedData(float *uncompressedData, unsigned int count, unsigned int magBits, unsigned int precBits);

float *get24BitDecompressedData(struct compressedVal *values, unsigned int count, unsigned int magBits, unsigned int precBits);

#endif