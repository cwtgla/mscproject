#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include "complete_compressor.h"
#include "zfp_example.h"

//struct to represent basic file stats
struct fileStats {
	int uncompressedCount;
	int runlengthCount;
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
	if(i == 0 || i == 250 || j == 0 || j == 250 || k == 0 || k == 50)
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


void transform(float *content) {
	int i = 0;
	int j = 0;
	int k = 0;

	int count = 0;
	float total = 0.0f;
	for(i = 0; i < 150; i++) {
		for(j = 0; j < 150; j++) {
			for(k = 0; k < 50; k++) {
				updateValue(i,j,k,content);
			}
		}
	}	
}

	int main() {
		char *directory = "../data/simulation_datasets/";
		char *files[100];
		int numFiles = 0;
		getAbsoluteFilepaths(files, directory, ".txt.clean", &numFiles);	//grab list of simulation datafiles

		int i;
		struct fileStats *stats = malloc(numFiles * sizeof(struct fileStats));
		float **datasets = malloc(numFiles*sizeof(float *));

		for(i = 0; i < numFiles; i++) {
			struct fileStats entry = { .maxVal = 0.0, .minVal = 0.0, .avgVal = 0.0, .uncompressedCount = 0, .runlengthCount = 0, .size24 = 0, .zfpSize = 0, .runlengthSize = 0};
			stats[i] = entry;
			datasets[i] = getData(files[i], &stats[i].uncompressedCount, &stats[i].maxVal, &stats[i].minVal, &stats[i].avgVal);
			printf("Basic stats for file: %s\nNumber of values: %d, Max value: %f, Min value: %f, Average value: %f\n", files[i], stats[i].uncompressedCount, stats[i].maxVal, stats[i].minVal, stats[i].avgVal);
			stats[i].runlengthSize = stats[i].uncompressedCount * sizeof(float);
			printf("\tUncompressed size: %lu bytes\n", stats[i].runlengthSize);
			printf("Stats after runlength compression\n");
			struct runlengthEntry *runlengthCompressed = getRunlengthCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].runlengthCount);
			printf("\tNumber of runlength entries: %d, Runlength compressed size: %lu bytes\n", stats[i].runlengthCount, stats[i].runlengthCount * sizeof(struct runlengthEntry));
			printf("Stats after ZFP compression\n");
			stats[i].zfpSize = zfpCompress(datasets[i], 150, 150, 90, 0.00, 0);
			printf("\tZFP compressed size: %lu bytes\n", stats[i].zfpSize);
			printf("Stats after 24 bit compression\n");
			struct compressedVal *compressed24 = get24BitCompressedData(datasets[i], stats[i].uncompressedCount, 5, 18);
			printf("24 Bit compressed size: %lu bytes\n", sizeof(struct compressedVal) * stats[i].uncompressedCount);
			printf("Stats for non byte aligned compression\n\n");
			//do non byte aligned compression
		}
		transform(datasets[0]);
		return 0;
	}
