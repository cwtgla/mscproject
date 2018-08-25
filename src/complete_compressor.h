#ifndef COMPLETE_COMPRESSOR_H_
#define COMPLETE_COMPRESSOR_H_

struct compressedVal { //struct to represent a multiple*8 bit compressed value
	unsigned char data[3];
};

void getAbsoluteFilepaths(char *files[], char *baseDirectory, char *fileExtension, int *count);

float *getData(char *absFilePath, int *dataLength);

int *getVerificationData(char *absFilePath, int *dataLength);

struct runlengthEntry *runlengthCompression(float *values, int count, int *newCount);

float *runlengthDecompression(struct runlengthEntry *compressedValues, int count, int *newCount);

struct compressedVal *get24BitCompressedData(float *uncompressedData, unsigned int count, unsigned int magBits, unsigned int precBits);

#endif