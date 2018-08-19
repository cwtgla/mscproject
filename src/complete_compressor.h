#ifndef COMPLETE_COMPRESSOR_H_
#define COMPLETE_COMPRESSOR_H_

void getAbsoluteFilepaths(char *files[], char *baseDirectory, char *fileExtension, int *count);

float *getData(char *absFilePath, int *dataLength);

struct runlengthEntry *runlengthCompression(float *values, int count, int *newCount);

float *runlengthDecompression(struct runlengthEntry *compressedValues, int count, int *newCount);

struct compressedVal* get24BitCompressedData(char *absFilePath, unsigned int magBits, unsigned int precBits);

#endif