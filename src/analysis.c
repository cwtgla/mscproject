#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include "compressor.h"
#include "zfp_example.h"


//float *uncompressedValues; //array for uncompressed values in dataset
struct fileStats *stats; //array for each files stats
float **datasets; //collection of all uncompressed datasets
struct compressedVal **compressed24Datasets;
int numDatasets;
int algorithm_repeat = 1;

//struct to represent basic file stats
struct fileStats {
	unsigned int uncompressedCount;
	unsigned int runlengthCount;
	unsigned int variableCount;
	long unsigned int runlengthSize;
	long unsigned int zfpSize;
	long unsigned int size24;
 	float maxVal;
 	float minVal;
 	float avgVal;
};

/*
 * Purpose:
 *		Get the index in the 1D array for the 3D coordinations
 * Parameters:
 *		1. i_rng - top index in i array
 *		2. j_rng - top index in j array
 *		3. i_lb - index for bottom of i array (0)
 *		4. j_lb - index for bottom of j array (0)
 *		5. k_lb - index for bottom of k array (0)
 *		6. ix - target i index
 *		7. jx - target j index
 *		8. kx - target k index
 * Returns:
 *		The 1d index
 */
inline unsigned int F3D2C(unsigned int i_rng, unsigned int j_rng, int i_lb, int j_lb, int k_lb, int ix, int jx, int kx) {
	return (i_rng*j_rng*(kx-k_lb)+i_rng*(jx-j_lb)+ix+i_lb);
}

/*
 * Purpose:
 *		Get the index for [i][j][k] in the 1d flat array
 * Parameters:
 *		1. i - index
 *		2. j - index
 *		3. k - index
 * Returns:
 *		The index value or -1 if the desired position falls off the array
 */
int getIndex(int i, int j, int k) {
	if(i == -1 || i == 150 || j == -1 || j == 150 || k == -1 || k == 90)
		return -1;
	else
		return F3D2C(150, 150, 0, 0, 0, i, j, k);
}

/*
 * Purpose:
 *		Update item in uncompressed float array given an index in 3-d space 
 * Parameters:
 *		1. i - index
 *		2. j - index
 *		3. k - index
 */
void updateUncompressedValue(int i, int j, int k) {
	float tmpValue;
	int divisor ;
	float currentValue;
	int fileInd;

	for(fileInd = 0; fileInd < numDatasets; fileInd++) {
		tmpValue = 0.0f;
		divisor = 0;
		currentValue = datasets[fileInd][F3D2C(150,150,0,0,0,i,j,k)];

		if(getIndex(i-1,j,k)!=-1) {
			tmpValue+=datasets[fileInd][F3D2C(150,150,0,0,0,i-1,j,k)];
			divisor++;
		}
		if(getIndex(i+1,j,k)!=-1){
			tmpValue+=datasets[fileInd][F3D2C(150,150,0,0,0,i+1,j,k)];
			divisor++;
		}
		if(getIndex(i,j-1,k) != -1){
			tmpValue+=datasets[fileInd][F3D2C(150,150,0,0,0,i,j-1,k)];
			divisor++;
		}
		if(getIndex(i,j+1,k)!=-1){
			tmpValue+=datasets[fileInd][F3D2C(150,150,0,0,0,i,j+1,k)];
			divisor++;
		}
		if(getIndex(i,j,k-1)!=-1){
			tmpValue+=datasets[fileInd][F3D2C(150,150,0,0,0,i,j,k-1)];
			divisor++;
		}
		if(getIndex(i,j,k+1)!=-1){
			tmpValue+=datasets[fileInd][F3D2C(150,150,0,0,0,i,j,k+1)];
			divisor++;
		}
		datasets[fileInd][F3D2C(150,150,0,0,0,i,j,k)]=currentValue+(tmpValue/divisor);
	}
}

/*
 * Purpose:
 *		Perform transformation algorithm on uncompressed floating point data and record performance
 */
void transformUncompressed() {
	int i, j, k, rep;
	clock_t startTime = clock();

	for(rep = 0; rep < algorithm_repeat; rep++) {
		for(i = 0; i < 150; i++) {
			for(j = 0; j < 150; j++) {
				for(k = 0; k < 90; k++) {
					updateUncompressedValue(i,j,k);
					updateUncompressedValue(i,j,k);
					updateUncompressedValue(i,j,k);
					updateUncompressedValue(i,j,k);
					updateUncompressedValue(i,j,k);
					updateUncompressedValue(i,j,k);
					updateUncompressedValue(i,j,k);
					updateUncompressedValue(i,j,k);
					updateUncompressedValue(i,j,k);
					updateUncompressedValue(i,j,k);
				}
			}
		}
	}

	clock_t endTime = clock();
	double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;

	printf("Time taken for algorithm on uncompressed data (averaged over %d interations)  = %f\n", algorithm_repeat, time_spent/algorithm_repeat);
}

/*
 * Purpose:
 *		Update a value in 24 bit format
 */
float update24BitCompressedValue(int i, int j, int k) {
	float tmpValue;
	int divisor;
	float currentValue;
	int fileInd;

	for(fileInd=0; fileInd < numDatasets; fileInd++) {
		tmpValue = 0.0f;
		divisor = 0;
		currentValue = getSingle24BitValue(compressed24Datasets[fileInd], F3D2C(150,150,0,0,0,i,j,k), 5, 18);

		if(getIndex(i-1,j,k)!=-1) {
			tmpValue+= getSingle24BitValue(compressed24Datasets[fileInd], F3D2C(150,150,0,0,0,i-1,j,k), 5, 18);
			divisor++;
		}
		if(getIndex(i+1,j,k)!=-1){
			tmpValue+= getSingle24BitValue(compressed24Datasets[fileInd], F3D2C(150,150,0,0,0,i+1,j,k), 5, 18);
			divisor++;
		}
		if(getIndex(i,j-1,k) != -1){
			tmpValue+= getSingle24BitValue(compressed24Datasets[fileInd], F3D2C(150,150,0,0,0,i,j-1,k), 5, 18);
			divisor++;
		}
		if(getIndex(i,j+1,k)!=-1){
			tmpValue+= getSingle24BitValue(compressed24Datasets[fileInd], F3D2C(150,150,0,0,0,i,j+1,k), 5, 18);
			divisor++;
		}
		if(getIndex(i,j,k-1)!=-1){
			tmpValue+= getSingle24BitValue(compressed24Datasets[fileInd], F3D2C(150,150,0,0,0,i,j,k-1), 5, 18);
			divisor++;
		}
		if(getIndex(i,j,k+1)!=-1){
			tmpValue+= getSingle24BitValue(compressed24Datasets[fileInd], F3D2C(150,150,0,0,0,i,j,k+1), 5, 18);
			divisor++;
		}
		insertSingle24BitValue(compressed24Datasets[fileInd], currentValue + (tmpValue/divisor), F3D2C(150,150,0,0,0,i,j,k), 5, 18);
	}
}

/*
 * Purpose:
 *		Perform transformation algorithm on 24 bit compressed data and record performance
 */
void transform24BitCompression() {
	int i, j, k, rep;
	clock_t startTime = clock();

	for(rep = 0; rep < algorithm_repeat; rep++) {
		for(i = 0; i < 150; i++) {
			for(j = 0; j < 150; j++) {
				for(k = 0; k < 90; k++) {
					printf("%d %d %d\n", i,j,k);
					update24BitCompressedValue(i,j,k);
					update24BitCompressedValue(i,j,k);
					update24BitCompressedValue(i,j,k);
					update24BitCompressedValue(i,j,k);
					update24BitCompressedValue(i,j,k);
					update24BitCompressedValue(i,j,k);
					update24BitCompressedValue(i,j,k);
					update24BitCompressedValue(i,j,k);
					update24BitCompressedValue(i,j,k);
					update24BitCompressedValue(i,j,k);
				}
			}
		}
	}

	clock_t endTime = clock();
	double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;

	printf("Time taken for algorithm on 24 bit compressed data (averaged over %d interations)  = %f\n", algorithm_repeat, time_spent/algorithm_repeat);
}

int main() {
	char *directory = "../data/simulation_datasets/";
	char *files[100];
	numDatasets = 0;
	getAbsoluteFilepaths(files, directory, ".txt.clean", &numDatasets);	//grab list of simulation datafiles

	stats = malloc(numDatasets * sizeof(struct fileStats));
	datasets = malloc(numDatasets * sizeof(float *));
	compressed24Datasets = malloc(numDatasets * sizeof(struct compressedVal *));

	int i;
	printf("Reading in datasets...\n"); //grab uncompressed data and generate stats
	for(i = 0; i < numDatasets; i++) {
		struct fileStats entry = { .maxVal = 0.0, .minVal = 0.0, .avgVal = 0.0, .variableCount = 0, .uncompressedCount = 0, .runlengthCount = 0, .size24 = 0, .zfpSize = 0, .runlengthSize = 0};
		stats[i] = entry;
		datasets[i] = getData(files[i], &stats[i].uncompressedCount, &stats[i].maxVal, &stats[i].minVal, &stats[i].avgVal);
		printf("Basic stats for file: %s\nNumber of values: %d, Max value: %f, Min value: %f, Average value: %f\n", files[i], stats[i].uncompressedCount, stats[i].maxVal, stats[i].minVal, stats[i].avgVal);
		printf("\tUncompressed size: %lu bytes\n", stats[i].uncompressedCount * sizeof(float));
		
		//runlength compression
		printf("Stats after runlength compression\n");
		struct runlengthEntry *runlengthCompressed = getRunlengthCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].runlengthCount);
		free(runlengthCompressed);
		printf("\tNumber of runlength entries: %d, Runlength compressed size: %lu bytes\n", stats[i].runlengthCount, stats[i].runlengthCount * sizeof(struct runlengthEntry));
		
		//zfp compression
		printf("Stats after ZFP compression\n");
		stats[i].zfpSize = zfpCompress(datasets[i], 150, 150, 90, 0.00, 0);
		printf("\tZFP compressed size: %lu bytes\n", stats[i].zfpSize);
		
		//24 bit compression
		printf("Stats after 24 bit compression\n");
		struct compressedVal *compressed24 = get24BitCompressedData(datasets[i], stats[i].uncompressedCount, 5, 18);
		compressed24Datasets[i] = compressed24;
		printf("\t24 Bit compressed size: %lu bytes\n", sizeof(struct compressedVal) * stats[i].uncompressedCount); //number of indexes doesnt change so no need for new value
		printf("Stats for non byte aligned compression\n");
		
		//do non byte aligned compression
		unsigned char *varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 15);
		printf("\t5 Mag 15 Precision compressed size: %lu\n", sizeof(unsigned char) * stats[i].variableCount);
		free(varCompressed);

		//do non byte aligned compression
		varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 12);
		printf("\t5 Mag 12 Precision compressed size: %lu\n", sizeof(unsigned char) * stats[i].variableCount);
		free(varCompressed);

		//do non byte aligned compression
		varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 9);
		printf("\t5 Mag 9 Precision compressed size: %lu\n", sizeof(unsigned char) * stats[i].variableCount);
		free(varCompressed);

		//do non byte aligned compression
		varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 6);
		printf("\t5 Mag 6 Precision compressed size: %lu\n\n\n", sizeof(unsigned char) * stats[i].variableCount);
		free(varCompressed);
	//	free(datasets[i]);
	}

	printf("Datasets read in!\n");
	printf("Running analysis\n");

	printf("Testing evaluating compression overhead...\n");
	transformUncompressed();
	transform24BitCompression();
}


// 		for(i = 0; i < numFiles; i++) {
// 			struct fileStats entry = { .maxVal = 0.0, .minVal = 0.0, .avgVal = 0.0, .variableCount = 0, .uncompressedCount = 0, .runlengthCount = 0, .size24 = 0, .zfpSize = 0, .runlengthSize = 0};
// 			stats[i] = entry;
// 			datasets[i] = getData(files[i], &stats[i].uncompressedCount, &stats[i].maxVal, &stats[i].minVal, &stats[i].avgVal);
// 			printf("Basic stats for file: %s\nNumber of values: %d, Max value: %f, Min value: %f, Average value: %f\n", files[i], stats[i].uncompressedCount, stats[i].maxVal, stats[i].minVal, stats[i].avgVal);
// 			stats[i].runlengthSize = stats[i].uncompressedCount * sizeof(float);
// 			printf("\tUncompressed size: %lu bytes\n", stats[i].runlengthSize);
			
// 			//runlength compression
// 			printf("Stats after runlength compression\n");
// 			struct runlengthEntry *runlengthCompressed = getRunlengthCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].runlengthCount);
// 			free(runlengthCompressed);
// 			printf("\tNumber of runlength entries: %d, Runlength compressed size: %lu bytes\n", stats[i].runlengthCount, stats[i].runlengthCount * sizeof(struct runlengthEntry));
			
// 			//zfp compression
// 			printf("Stats after ZFP compression\n");
// 			stats[i].zfpSize = zfpCompress(datasets[i], 150, 150, 90, 0.00, 0);
// 			printf("\tZFP compressed size: %lu bytes\n", stats[i].zfpSize);
			
// 			//24 bit compression
// 			printf("Stats after 24 bit compression\n");
// 			struct compressedVal *compressed24 = get24BitCompressedData(datasets[i], stats[i].uncompressedCount, 5, 18);
// 			free(compressed24);
// 			printf("\t24 Bit compressed size: %lu bytes\n", sizeof(struct compressedVal) * stats[i].uncompressedCount);
// 			printf("Stats for non byte aligned compression\n");
			
// 			//do non byte aligned compression
// 			unsigned char *varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 15);
// 			printf("\t5 Mag 15 Precision compressed size: %lu\n\n\n", sizeof(unsigned char) * stats[i].variableCount);
// 			free(varCompressed);

// 			//do non byte aligned compression
// 			varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 12);
// 			printf("\t5 Mag 12 Precision compressed size: %lu\n\n\n", sizeof(unsigned char) * stats[i].variableCount);
// 			free(varCompressed);

// 			//do non byte aligned compression
// 			varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 9);
// 			printf("\t5 Mag 9 Precision compressed size: %lu\n\n\n", sizeof(unsigned char) * stats[i].variableCount);
// 			free(varCompressed);

// 			//do non byte aligned compression
// 			varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 6);
// 			printf("\t5 Mag 6 Precision compressed size: %lu\n\n\n", sizeof(unsigned char) * stats[i].variableCount);
// 			free(varCompressed);
// 			free(datasets[i]);
// 		}
// }