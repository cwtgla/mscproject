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
unsigned char **lossy21;
unsigned char **lossy18;
unsigned char **lossy15;
unsigned char **lossy12;
int numDatasets;
int algorithm_repeat = 1;
int repeat = 1;

//struct to represent basic file stats
struct fileStats {
	unsigned int uncompressedCount;
	unsigned int runlengthCount;
	unsigned int variableCount;
	unsigned int var21Count;
	unsigned int var18Count;
	unsigned int var15Count;
	unsigned int var12Count;
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
 *		Update a value in 21 bit format
 */
float update21BitCompressedValue(int i, int j, int k) {
	float tmpValue;
	int divisor;
	float currentValue;
	int fileInd;

	for(fileInd=0; fileInd < numDatasets; fileInd++) {
		tmpValue = 0.0f;
		divisor = 0;
		currentValue = getSingleVariableBitValue(lossy21[fileInd], stats[fileInd].var21Count, F3D2C(150,150,0,0,0,i,j,k), 5, 15);

		if(getIndex(i-1,j,k)!=-1) {
			tmpValue+= getSingleVariableBitValue(lossy21[fileInd], stats[fileInd].var21Count, F3D2C(150,150,0,0,0,i-1,j,k), 5, 15);
			divisor++;
		}
		if(getIndex(i+1,j,k)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy21[fileInd], stats[fileInd].var21Count,  F3D2C(150,150,0,0,0,i+1,j,k), 5, 15);
			divisor++;
		}
		if(getIndex(i,j-1,k) != -1){
			tmpValue+= getSingleVariableBitValue(lossy21[fileInd], stats[fileInd].var21Count,  F3D2C(150,150,0,0,0,i,j-1,k), 5, 15);
			divisor++;
		}
		if(getIndex(i,j+1,k)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy21[fileInd], stats[fileInd].var21Count,  F3D2C(150,150,0,0,0,i,j+1,k), 5, 15);
			divisor++;
		}
		if(getIndex(i,j,k-1)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy21[fileInd], stats[fileInd].var21Count,  F3D2C(150,150,0,0,0,i,j,k-1), 5, 15);
			divisor++;
		}
		if(getIndex(i,j,k+1)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy21[fileInd], stats[fileInd].var21Count,  F3D2C(150,150,0,0,0,i,j,k+1), 5, 15);
			divisor++;
		}
		insertSingleVariableBitValue(lossy21[fileInd], stats[fileInd].var21Count, F3D2C(150,150,0,0,0,i,j,k), currentValue + (tmpValue/divisor), 5, 15);
	}
}

/*
 * Purpose:
 *		Update a value in 18 bit format
 */
float update18BitCompressedValue(int i, int j, int k) {
	float tmpValue;
	int divisor;
	float currentValue;
	int fileInd;

	for(fileInd=0; fileInd < numDatasets; fileInd++) {
		tmpValue = 0.0f;
		divisor = 0;
		currentValue = getSingleVariableBitValue(lossy18[fileInd],stats[fileInd].var18Count,  F3D2C(150,150,0,0,0,i,j,k), 5, 12);

		if(getIndex(i-1,j,k)!=-1) {
			tmpValue+= getSingleVariableBitValue(lossy18[fileInd], stats[fileInd].var18Count, F3D2C(150,150,0,0,0,i-1,j,k), 5, 12);
			divisor++;
		}
		if(getIndex(i+1,j,k)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy18[fileInd], stats[fileInd].var18Count, F3D2C(150,150,0,0,0,i+1,j,k), 5, 12);
			divisor++;
		}
		if(getIndex(i,j-1,k) != -1){
			tmpValue+= getSingleVariableBitValue(lossy18[fileInd], stats[fileInd].var18Count, F3D2C(150,150,0,0,0,i,j-1,k), 5, 12);
			divisor++;
		}
		if(getIndex(i,j+1,k)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy18[fileInd], stats[fileInd].var18Count,  F3D2C(150,150,0,0,0,i,j+1,k), 5, 12);
			divisor++;
		}
		if(getIndex(i,j,k-1)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy18[fileInd], stats[fileInd].var18Count,  F3D2C(150,150,0,0,0,i,j,k-1), 5, 12);
			divisor++;
		}
		if(getIndex(i,j,k+1)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy18[fileInd], stats[fileInd].var18Count, F3D2C(150,150,0,0,0,i,j,k+1), 5, 12);
			divisor++;
		}
		insertSingleVariableBitValue(lossy18[fileInd], stats[fileInd].var18Count, F3D2C(150,150,0,0,0,i,j,k), currentValue + (tmpValue/divisor), 5, 12);
	}
}

/*
 * Purpose:
 *		Update a value in 15 bit format
 */
float update15BitCompressedValue(int i, int j, int k) {
	float tmpValue;
	int divisor;
	float currentValue;
	int fileInd;

	for(fileInd=0; fileInd < numDatasets; fileInd++) {
		tmpValue = 0.0f;
		divisor = 0;
		currentValue = getSingleVariableBitValue(lossy15[fileInd], stats[fileInd].var15Count, F3D2C(150,150,0,0,0,i,j,k), 5, 9);

		if(getIndex(i-1,j,k)!=-1) {
			tmpValue+= getSingleVariableBitValue(lossy15[fileInd], stats[fileInd].var15Count, F3D2C(150,150,0,0,0,i-1,j,k), 5, 9);
			divisor++;
		}
		if(getIndex(i+1,j,k)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy15[fileInd], stats[fileInd].var15Count, F3D2C(150,150,0,0,0,i+1,j,k), 5, 9);
			divisor++;
		}
		if(getIndex(i,j-1,k) != -1){
			tmpValue+= getSingleVariableBitValue(lossy15[fileInd], stats[fileInd].var15Count, F3D2C(150,150,0,0,0,i,j-1,k), 5, 9);
			divisor++;
		}
		if(getIndex(i,j+1,k)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy15[fileInd], stats[fileInd].var15Count, F3D2C(150,150,0,0,0,i,j+1,k), 5, 9);
			divisor++;
		}
		if(getIndex(i,j,k-1)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy15[fileInd], stats[fileInd].var15Count, F3D2C(150,150,0,0,0,i,j,k-1), 5, 9);
			divisor++;
		}
		if(getIndex(i,j,k+1)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy15[fileInd], stats[fileInd].var15Count, F3D2C(150,150,0,0,0,i,j,k+1), 5, 9);
			divisor++;
		}
		insertSingleVariableBitValue(lossy15[fileInd], stats[fileInd].var15Count, F3D2C(150,150,0,0,0,i,j,k), currentValue + (tmpValue/divisor), 5, 9);
	}
}

/*
 * Purpose:
 *		Update a value in 12 bit format
 */
float update12BitCompressedValue(int i, int j, int k) {
	float tmpValue;
	int divisor;
	float currentValue;
	int fileInd;

	for(fileInd=0; fileInd < numDatasets; fileInd++) {
		tmpValue = 0.0f;
		divisor = 0;
		currentValue = getSingleVariableBitValue(lossy12[fileInd], stats[fileInd].var12Count, F3D2C(150,150,0,0,0,i,j,k), 5, 6);

		if(getIndex(i-1,j,k)!=-1) {
			tmpValue+= getSingleVariableBitValue(lossy12[fileInd], stats[fileInd].var12Count, F3D2C(150,150,0,0,0,i-1,j,k), 5, 6);
			divisor++;
		}
		if(getIndex(i+1,j,k)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy12[fileInd], stats[fileInd].var12Count, F3D2C(150,150,0,0,0,i+1,j,k), 5, 6);
			divisor++;
		}
		if(getIndex(i,j-1,k) != -1){
			tmpValue+= getSingleVariableBitValue(lossy12[fileInd], stats[fileInd].var12Count, F3D2C(150,150,0,0,0,i,j-1,k), 5, 6);
			divisor++;
		}
		if(getIndex(i,j+1,k)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy12[fileInd], stats[fileInd].var12Count, F3D2C(150,150,0,0,0,i,j+1,k), 5, 6);
			divisor++;
		}
		if(getIndex(i,j,k-1)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy12[fileInd], stats[fileInd].var12Count, F3D2C(150,150,0,0,0,i,j,k-1), 5, 6);
			divisor++;
		}
		if(getIndex(i,j,k+1)!=-1){
			tmpValue+= getSingleVariableBitValue(lossy12[fileInd], stats[fileInd].var12Count, F3D2C(150,150,0,0,0,i,j,k+1), 5, 6);
			divisor++;
		}
		insertSingleVariableBitValue(lossy12[fileInd], stats[fileInd].var12Count, F3D2C(150,150,0,0,0,i,j,k), currentValue + (tmpValue/divisor), 5, 6);
	}
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
					// updateUncompressedValue(i,j,k);
					// updateUncompressedValue(i,j,k);
					// updateUncompressedValue(i,j,k);
					// updateUncompressedValue(i,j,k);
					// updateUncompressedValue(i,j,k);
					// updateUncompressedValue(i,j,k);
					// updateUncompressedValue(i,j,k);
					// updateUncompressedValue(i,j,k);
					// updateUncompressedValue(i,j,k);
				}
			}
		}
	}

	clock_t endTime = clock();
	double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;

	printf("Time taken for algorithm on uncompressed data (averaged over %d interations)  = %f\n", algorithm_repeat, time_spent/algorithm_repeat);
}

void transformNonByteAligned12Compression() {
	int i, j, k, rep;
	clock_t startTime = clock();

	for(rep = 0; rep < algorithm_repeat; rep++) {
		for(i = 0; i < 150; i++) {
			for(j = 0; j < 150; j++) {
				for(k = 0; k < 90; k++) {
					update12BitCompressedValue(i,j,k);
					// update12BitCompressedValue(i,j,k);
					// update12BitCompressedValue(i,j,k);
					// update12BitCompressedValue(i,j,k);
					// update12BitCompressedValue(i,j,k);
					// update12BitCompressedValue(i,j,k);
					// update12BitCompressedValue(i,j,k);
					// update12BitCompressedValue(i,j,k);
					// update12BitCompressedValue(i,j,k);
					// update12BitCompressedValue(i,j,k);
				}
			}
		}
	}

	clock_t endTime = clock();
	double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;

	printf("Time taken for algorithm on 12 bit compressed data (averaged over %d interations)  = %f\n", algorithm_repeat, time_spent/algorithm_repeat);
}

void transformNonByteAligned15Compression() {
	int i, j, k, rep;
	clock_t startTime = clock();

	for(rep = 0; rep < algorithm_repeat; rep++) {
		for(i = 0; i < 150; i++) {
			for(j = 0; j < 150; j++) {
				for(k = 0; k < 90; k++) {
					update15BitCompressedValue(i,j,k);
					// update15BitCompressedValue(i,j,k);
					// update15BitCompressedValue(i,j,k);
					// update15BitCompressedValue(i,j,k);
					// update15BitCompressedValue(i,j,k);
					// update15BitCompressedValue(i,j,k);
					// update15BitCompressedValue(i,j,k);
					// update15BitCompressedValue(i,j,k);
					// update15BitCompressedValue(i,j,k);
					// update15BitCompressedValue(i,j,k);
				}
			}
		}
	}

	clock_t endTime = clock();
	double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;

	printf("Time taken for algorithm on 15 bit compressed data (averaged over %d interations)  = %f\n", algorithm_repeat, time_spent/algorithm_repeat);
}

void transformNonByteAligned18Compression() {
	int i, j, k, rep;
	clock_t startTime = clock();

	for(rep = 0; rep < algorithm_repeat; rep++) {
		for(i = 0; i < 150; i++) {
			for(j = 0; j < 150; j++) {
				for(k = 0; k < 90; k++) {
					update18BitCompressedValue(i,j,k);
					// update18BitCompressedValue(i,j,k);
					// update18BitCompressedValue(i,j,k);
					// update18BitCompressedValue(i,j,k);
					// update18BitCompressedValue(i,j,k);
					// update18BitCompressedValue(i,j,k);
					// update18BitCompressedValue(i,j,k);
					// update18BitCompressedValue(i,j,k);
					// update18BitCompressedValue(i,j,k);
					// update18BitCompressedValue(i,j,k);
				}
			}
		}
	}

	clock_t endTime = clock();
	double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;

	printf("Time taken for algorithm on 18 bit compressed data (averaged over %d interations)  = %f\n", algorithm_repeat, time_spent/algorithm_repeat);
}

void transformNonByteAligned21Compression() {
	int i, j, k, rep;
	clock_t startTime = clock();

	for(rep = 0; rep < algorithm_repeat; rep++) {
		for(i = 0; i < 150; i++) {
			for(j = 0; j < 150; j++) {
				for(k = 0; k < 90; k++) {
					//printf("%d %d %d\n", i,j,k);
					update21BitCompressedValue(i,j,k);
					// update21BitCompressedValue(i,j,k);
					// update21BitCompressedValue(i,j,k);
					// update21BitCompressedValue(i,j,k);
					// update21BitCompressedValue(i,j,k);
					// update21BitCompressedValue(i,j,k);
					// update21BitCompressedValue(i,j,k);
					// update21BitCompressedValue(i,j,k);
					// update21BitCompressedValue(i,j,k);
					// update21BitCompressedValue(i,j,k);
				}
			}
		}
	}

	clock_t endTime = clock();
	double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;

	printf("Time taken for algorithm on 21 bit compressed data (averaged over %d interations)  = %f\n", algorithm_repeat, time_spent/algorithm_repeat);
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
					//printf("%d %d %d\n", i,j,k);
					update24BitCompressedValue(i,j,k);
					// update24BitCompressedValue(i,j,k);
					// update24BitCompressedValue(i,j,k);
					// update24BitCompressedValue(i,j,k);
					// update24BitCompressedValue(i,j,k);
					// update24BitCompressedValue(i,j,k);
					// update24BitCompressedValue(i,j,k);
					// update24BitCompressedValue(i,j,k);
					// update24BitCompressedValue(i,j,k);
					// update24BitCompressedValue(i,j,k);
				}
			}
		}
	}

	clock_t endTime = clock();
	double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;

	printf("Time taken for algorithm on 24 bit compressed data (averaged over %d interations)  = %f\n", algorithm_repeat, time_spent/algorithm_repeat);
}

void compressionSpeedAnalysis() {
	int i;
	double rlTotal = 0;
	double zfpTotal = 0;
	double lossy24BitTotal = 0;
	double lossy21BitTotal = 0;
	float lossy18BitTotal = 0;
	double lossy15BitTotal = 0;
	double lossy12BitTotal = 0;
	clock_t start;
	int rep;

	for(rep = 0; rep < repeat; rep++) {
		for(i = 0; i < numDatasets; i++) {
			start = clock();
			getRunlengthCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].runlengthCount);
			rlTotal+= (double)(clock() - start);

			start = clock();
			zfpCompress(datasets[i], 150, 150, 90, 0.00, 0);
			zfpTotal+= (double)(clock() - start);

			start = clock();
			get24BitCompressedData(datasets[i], stats[i].uncompressedCount, 5, 18);
			lossy24BitTotal+= (double) (clock() - start);

			start = clock();
			getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 15);
			lossy21BitTotal+= (double) (clock() - start);

			start = clock();
			getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 12);
			lossy18BitTotal+= (double) (clock() - start);

			start = clock();
			getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 9);
			lossy15BitTotal+= (double) (clock() - start);

			start = clock();
			getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 6);
			lossy12BitTotal+= (double) (clock() - start);
		}
	}
	printf("Average compression times\n");
	printf("Runlength: %f seconds\n", (rlTotal/numDatasets*repeat)/CLOCKS_PER_SEC);
	printf("ZFP: %f seconds\n", (zfpTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
	printf("24 Bit Lossy: %f seconds\n", (lossy24BitTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
	printf("21 Bit Lossy: %f seconds\n", (lossy21BitTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
	printf("18 Bit Lossy: %f seconds\n", (lossy18BitTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
	printf("15 Bit Lossy: %f seconds\n", (lossy15BitTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
	printf("12 Bit Lossy: %f seconds\n", (lossy12BitTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
}

void decompressionSpeedAnalysis() {
	//for each dataset, compress it once then decompress and time
	int i;
	double rlTotal = 0;
	double zfpTotal = 0;
	double lossy24BitTotal = 0;
	double lossy21BitTotal = 0;
	float lossy18BitTotal = 0;
	double lossy15BitTotal = 0;
	double lossy12BitTotal = 0;
	clock_t start;
	int rep;

	struct runlengthEntry **rlComp = malloc(numDatasets*sizeof(struct runlengthEntry *));
	struct compressedVal **compressed24 = malloc(numDatasets*sizeof(struct compressedVal *));
	unsigned char **compressed21 = malloc(numDatasets * sizeof(unsigned char *));
	unsigned char **compressed18 = malloc(numDatasets * sizeof(unsigned char *));
	unsigned char **compressed15 = malloc(numDatasets * sizeof(unsigned char *));
	unsigned char **compressed12 = malloc(numDatasets * sizeof(unsigned char *));

	//generate compressed versions to decompress
	for(i = 0; i < numDatasets; i++) {
		rlComp[i] = getRunlengthCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].runlengthCount);
		compressed24[i] = get24BitCompressedData(datasets[i], stats[i].uncompressedCount, 5, 18);
		compressed21[i] = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].var21Count, 5, 15);
		compressed18[i] = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].var18Count, 5, 12);
		compressed15[i] = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].var15Count, 5, 9);
		compressed12[i] = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].var12Count, 5, 6);
	}
	unsigned int junk = 0;

	for(rep = 0; rep < repeat; rep++) {
		for(i = 0; i < numDatasets; i++) {
			start = clock();
			getRunlengthDecompressedData(rlComp[i], stats[i].runlengthSize, &junk);
			rlTotal+= (double) (clock() - start);

			start = clock();
			get24BitDecompressedData(compressed24[i], stats[i].uncompressedCount, 5, 18);
			lossy24BitTotal+= (double) (clock() - start);

			start = clock();
			getVariableBitDecompressedData(compressed21[i], stats[i].uncompressedCount, &junk, 5, 15);
			lossy21BitTotal+= (double) (clock() - start);

			start = clock();
			getVariableBitDecompressedData(compressed18[i], stats[i].uncompressedCount, &junk, 5, 12);
			lossy18BitTotal+= (double) (clock() - start);

			start = clock();
			getVariableBitDecompressedData(compressed15[i], stats[i].uncompressedCount, &junk, 5, 9);
			lossy15BitTotal+= (double) (clock() - start);

			start = clock();
			getVariableBitDecompressedData(compressed12[i], stats[i].uncompressedCount, &junk, 5, 6);
			lossy12BitTotal+= (double) (clock() - start);
		}
	}
	printf("Average Decompression times\n");
	printf("Runlength: %f seconds\n", (rlTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
	printf("24 Bit Lossy: %f seconds\n", (lossy24BitTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
	printf("21 Bit Lossy: %f seconds\n", (lossy21BitTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
	printf("18 Bit Lossy: %f seconds\n", (lossy18BitTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
	printf("15 Bit Lossy: %f seconds\n", (lossy15BitTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
	printf("12 Bit Lossy: %f seconds\n", (lossy12BitTotal/(numDatasets*repeat))/CLOCKS_PER_SEC);
}

int main() {
	char *directory = "../data/simulation_datasets/";
	char *files[100];
	numDatasets = 0;
	getAbsoluteFilepaths(files, directory, ".txt.clean", &numDatasets);	//grab list of simulation datafiles

	stats = malloc(numDatasets * sizeof(struct fileStats));
	datasets = malloc(numDatasets * sizeof(float *));
	compressed24Datasets = malloc(numDatasets * sizeof(struct compressedVal *));
	lossy21 = malloc(numDatasets * sizeof(unsigned char *));
	lossy18 = malloc(numDatasets * sizeof(unsigned char *));
	lossy15 = malloc(numDatasets * sizeof(unsigned char *));
	lossy12 = malloc(numDatasets * sizeof(unsigned char *));
	
	int i;
	printf("Reading in datasets...\n"); //grab uncompressed data and generate stats
	for(i = 0; i < numDatasets; i++) {
		struct fileStats entry = { .maxVal = 0.0, .minVal = 0.0, .avgVal = 0.0, .variableCount = 0, .var21Count=0, .var18Count = 0, .var15Count = 0, .var12Count = 0, .uncompressedCount = 0, .runlengthCount = 0, .size24 = 0, .zfpSize = 0, .runlengthSize = 0};
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
		lossy21[i] = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].var21Count, 5, 15);
		printf("\t5 Mag 15 Precision compressed size: %lu\n", sizeof(unsigned char) * stats[i].var21Count);
		//free(varCompressed);

		//do non byte aligned compression
		lossy18[i] = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].var18Count, 5, 12);
		printf("\t5 Mag 12 Precision compressed size: %lu\n", sizeof(unsigned char) * stats[i].var18Count);
		//free(varCompressed);

		//do non byte aligned compression
		lossy15[i] = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].var15Count, 5, 9);
		printf("\t5 Mag 9 Precision compressed size: %lu\n", sizeof(unsigned char) * stats[i].var15Count);
		//free(varCompressed);

		//do non byte aligned compression
		lossy12[i] = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].var12Count, 5, 6);
		printf("\t5 Mag 6 Precision compressed size: %lu\n\n\n", sizeof(unsigned char) * stats[i].var12Count);
		//free(varCompressed);
		//free(datasets[i]);
	}

	printf("Running compression speed tests\n");
	compressionSpeedAnalysis();
	decompressionSpeedAnalysis();
	printf("\n");

	printf("Datasets read in!\n");
	printf("Running analysis\n");
	printf("Testing evaluating compression overhead...\n");
	transformUncompressed();
	transform24BitCompression();
	transformNonByteAligned21Compression();
	transformNonByteAligned18Compression();
	transformNonByteAligned15Compression();
	transformNonByteAligned12Compression();
}
