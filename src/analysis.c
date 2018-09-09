#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include "compressor.h"
#include "zfp_example.h"

#if defined(WIN32) || defined(_WIN32) 
#define PATH_SEPARATOR "\\" 
#else 
#define PATH_SEPARATOR "/" 
#endif 

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

/* convert from i,j,k notation for a 3D array to a flat index
 * rng - ranges i.e. hl-lb+1
 * lb - lower bounds
 */
inline unsigned int F3D2C(unsigned int i_rng, unsigned int j_rng, int i_lb, int j_lb, int k_lb, int ix, int jx, int kx) {
	return (i_rng*j_rng*(kx-k_lb)+i_rng*(jx-j_lb)+ix+i_lb);
}

//Return index of the value or -1 if its invalid
int getIndex(int i, int j, int k) {
	if(i == -1 || i == 150 || j == -1 || j == 150 || k == -1 || k == 90)
		return -1;
	else
		return F3D2C(150, 150, 0, 0, 0, i, j, k);
}

//given an i,j,k index update the value based on whats around it
float updateValue(int i, int j, int k, float* values) {
	float tmp = 0.0f;
	int divisor = 0;
	float current = values[F3D2C(150,150,0,0,0,i,j,k)];

	if(getIndex(i-1,j,k)!=-1) {
		tmp+=values[F3D2C(150,150,0,0,0,i-1,j,k)];
		divisor++;
	}
	if(getIndex(i+1,j,k)!=-1){
		tmp+=values[F3D2C(150,150,0,0,0,i+1,j,k)];
		divisor++;
	}
	if(getIndex(i,j-1,k) != -1){
		tmp+=values[F3D2C(150,150,0,0,0,i,j-1,k)];
		divisor++;
	}
	if(getIndex(i,j+1,k)!=-1){
		tmp+=values[F3D2C(150,150,0,0,0,i,j+1,k)];
		divisor++;
	}
	if(getIndex(i,j,k-1)!=-1){
		tmp+=values[F3D2C(150,150,0,0,0,i,j,k-1)];
		divisor++;
	}
	if(getIndex(i,j,k+1)!=-1){
		tmp+=values[F3D2C(150,150,0,0,0,i,j,k+1)];
		divisor++;
	}
	values[F3D2C(150,150,0,0,0,i,j,k)]=current+(tmp/divisor);
}



void uncompressedTransformation() {
		char *directory = "../data/simulation_datasets/";
		char *files[100];
		int numFiles = 0;
		getAbsoluteFilepaths(files, directory, ".txt.clean", &numFiles);	//grab list of simulation datafiles

		int i;
		struct fileStats *stats = malloc(numFiles * sizeof(struct fileStats));
		float **datasets = malloc(numFiles*sizeof(float *));

		//grab datasets
		for(i = 0; i < numFiles; i++) {
			struct fileStats entry = { .maxVal = 0.0, .minVal = 0.0, .avgVal = 0.0, .variableCount = 0, .uncompressedCount = 0, .runlengthCount = 0, .size24 = 0, .zfpSize = 0, .runlengthSize = 0};
			stats[i] = entry;
			datasets[i] = getData(files[i], &stats[i].uncompressedCount, &stats[i].maxVal, &stats[i].minVal, &stats[i].avgVal);
		}

		int j;
		int k;
		int rep;

	clock_t start = clock();

	//for(rep=0; rep < 10; rep++) {
		for(i = 0; i < 150; i++) {
			for(j = 0; j < 150; j++) {
				for(k = 0; k < 90; k++) {
					updateValue(i,j,k,datasets[0]);
					updateValue(i,j,k,datasets[1]);
					updateValue(i,j,k,datasets[2]);
					updateValue(i,j,k,datasets[3]);
					updateValue(i,j,k,datasets[4]);
					updateValue(i,j,k,datasets[5]);
					updateValue(i,j,k,datasets[6]);
					updateValue(i,j,k,datasets[7]);
					updateValue(i,j,k,datasets[8]);
					updateValue(i,j,k,datasets[9]);
				}
			}
		}
	//}
	clock_t end = clock();
	double time_spend = (double)(end-start) / CLOCKS_PER_SEC;
	//printf("time taken uncompressed = %f %f\n", time_spend, time_spend/10.0);
	printf("time taken 24compressed = %f\n", time_spend);
	free(stats);
	free(datasets);
}

///////////////////////////////////////////////////

//given an i,j,k index update the value based on whats around it
float update24Value(int i, int j, int k, struct compressedVal *values) {
	float tmp = 0.0f;
	int divisor = 0;
	//float current = values[F3D2C(150,150,0,0,0,i,j,k)];
	float current = getSingle24BitValue(values, F3D2C(150,150,0,0,0,i,j,k), 5, 18);

	if(getIndex(i-1,j,k)!=-1) {
		tmp+=getSingle24BitValue(values, F3D2C(150,150,0,0,0,i-1,j,k), 5, 18);
		divisor++;
	}
	if(getIndex(i+1,j,k)!=-1){
		tmp+=getSingle24BitValue(values, F3D2C(150,150,0,0,0,i+1,j,k), 5, 18);
		//tmp+=values[F3D2C(150,150,0,0,0,i+1,j,k)];
		divisor++;
	}
	if(getIndex(i,j-1,k) != -1){
		tmp+=getSingle24BitValue(values, F3D2C(150,150,0,0,0,i,j-1,k), 5, 18);
	//	tmp+=values[F3D2C(150,150,0,0,0,i,j-1,k)];
		divisor++;
	}
	if(getIndex(i,j+1,k)!=-1){
		tmp+=getSingle24BitValue(values, F3D2C(150,150,0,0,0,i,j+1,k), 5, 18);
		//tmp+=values[F3D2C(150,150,0,0,0,i,j+1,k)];
		divisor++;
	}
	if(getIndex(i,j,k-1)!=-1){
		tmp+=getSingle24BitValue(values, F3D2C(150,150,0,0,0,i,j,k-1), 5, 18);
	//	tmp+=values[F3D2C(150,150,0,0,0,i,j,k-1)];
		divisor++;
	}
	if(getIndex(i,j,k+1)!=-1){
		tmp+=getSingle24BitValue(values, F3D2C(150,150,0,0,0,i,j,k+1), 5, 18);
		//tmp+=values[F3D2C(150,150,0,0,0,i,j,k+1)];
		divisor++;
	}
	insertSingle24BitValue(values, current+(tmp/divisor), F3D2C(150,150,0,0,0,i,j,k), 5, 18);
	//values[F3D2C(150,150,0,0,0,i,j,k)]=current+(tmp/divisor);
}


void transform24() {
		char *directory = "../data/simulation_datasets/";
		char *files[100];
		int numFiles = 0;
		getAbsoluteFilepaths(files, directory, ".txt.clean", &numFiles);	//grab list of simulation datafiles

		int i;
		struct fileStats *stats = malloc(numFiles * sizeof(struct fileStats));
		//float **datasets = malloc(numFiles*sizeof(float *));
		//struct compressedVal **datasets 
		struct compressedVal **datasets = malloc(numFiles*sizeof(struct compressedVal *));

		//grab datasets
		for(i = 0; i < numFiles; i++) {
			struct fileStats entry = { .maxVal = 0.0, .minVal = 0.0, .avgVal = 0.0, .variableCount = 0, .uncompressedCount = 0, .runlengthCount = 0, .size24 = 0, .zfpSize = 0, .runlengthSize = 0};
			stats[i] = entry;
			float *data = getData(files[i], &stats[i].uncompressedCount, &stats[i].maxVal, &stats[i].minVal, &stats[i].avgVal);
			struct compressedVal *compressed24 = get24BitCompressedData(data, stats[i].uncompressedCount, 5, 18);
			datasets[i] = compressed24;
		}

		int j;
		int k;
		int rep;

	clock_t start = clock();

	//for(rep=0; rep < 10; rep++) {
		for(i = 0; i < 150; i++) {
			for(j = 0; j < 150; j++) {
				for(k = 0; k < 90; k++) {
					update24Value(i,j,k,datasets[0]);
					update24Value(i,j,k,datasets[1]);
					update24Value(i,j,k,datasets[2]);
					update24Value(i,j,k,datasets[3]);
					update24Value(i,j,k,datasets[4]);
					update24Value(i,j,k,datasets[5]);
					update24Value(i,j,k,datasets[6]);
					update24Value(i,j,k,datasets[7]);
					update24Value(i,j,k,datasets[8]);
					update24Value(i,j,k,datasets[9]);
				}
			}
		}
//	}
	clock_t end = clock();
	double time_spend = (double)(end-start) / CLOCKS_PER_SEC;
	printf("time taken 24compressed = %f\n", time_spend);
	//printf("time taken 24compressed = %f %f\n", time_spend, time_spend/10.0);
}


void compressionRatioAnalysis() {
		char *directory = "../data/simulation_datasets/";
		char *files[100];
		int numFiles = 0;
		getAbsoluteFilepaths(files, directory, ".txt.clean", &numFiles);	//grab list of simulation datafiles

		int i;
		struct fileStats *stats = malloc(numFiles * sizeof(struct fileStats));
		float **datasets = malloc(numFiles*sizeof(float *));

		for(i = 0; i < numFiles; i++) {
			struct fileStats entry = { .maxVal = 0.0, .minVal = 0.0, .avgVal = 0.0, .variableCount = 0, .uncompressedCount = 0, .runlengthCount = 0, .size24 = 0, .zfpSize = 0, .runlengthSize = 0};
			stats[i] = entry;
			datasets[i] = getData(files[i], &stats[i].uncompressedCount, &stats[i].maxVal, &stats[i].minVal, &stats[i].avgVal);
			printf("Basic stats for file: %s\nNumber of values: %d, Max value: %f, Min value: %f, Average value: %f\n", files[i], stats[i].uncompressedCount, stats[i].maxVal, stats[i].minVal, stats[i].avgVal);
			stats[i].runlengthSize = stats[i].uncompressedCount * sizeof(float);
			printf("\tUncompressed size: %lu bytes\n", stats[i].runlengthSize);
			
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
			free(compressed24);
			printf("\t24 Bit compressed size: %lu bytes\n", sizeof(struct compressedVal) * stats[i].uncompressedCount);
			printf("Stats for non byte aligned compression\n");
			
			//do non byte aligned compression
			unsigned char *varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 15);
			printf("\t5 Mag 15 Precision compressed size: %lu\n\n\n", sizeof(unsigned char) * stats[i].variableCount);
			free(varCompressed);

			//do non byte aligned compression
			varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 12);
			printf("\t5 Mag 12 Precision compressed size: %lu\n\n\n", sizeof(unsigned char) * stats[i].variableCount);
			free(varCompressed);

			//do non byte aligned compression
			varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 9);
			printf("\t5 Mag 9 Precision compressed size: %lu\n\n\n", sizeof(unsigned char) * stats[i].variableCount);
			free(varCompressed);

			//do non byte aligned compression
			varCompressed = getVariableBitCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].variableCount, 5, 6);
			printf("\t5 Mag 6 Precision compressed size: %lu\n\n\n", sizeof(unsigned char) * stats[i].variableCount);
			free(varCompressed);
			free(datasets[i]);
		}
}

	//run options are compressionRatio analysis or compression overhead analysis
	int main() {
		//compressionRatioAnalysis();
		//uncompressedTransformation();
		transform24();
		//make arrays global? faster code

		return 0;
	}