#ifndef COMPLETE_COMPRESSOR_H_
#define COMPLETE_COMPRESSOR_H_

void getAbsoluteFilepaths(char *files[], char *baseDirectory, char *fileExtension, int *count);

float *getData(char *absFilePath, int *dataLength);

struct runlengthEntry *runlengthCompression(float *values, int count, int *newCount);
#endif
