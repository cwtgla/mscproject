#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include "complete_compressor.h"
#include "zfp_example.c"

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



// //Get basic file stats (max,min,mean and number of values)
// void displayStats(char *filePath) {
// 	FILE *contentFile;
// //	printf("After rounding:%f ", value);
// 	//printf("%X\n", &value);
// 	float fraction, intpart;

//	fraction = modff(value, &intpart);
//	printf("Int part:%f fraction:%f\n", intpart, fraction);
//	int firstPart = (int)intpart;
//	int secondPart  = (int)(fraction*10000);
//	printf("To store, before DP %d, after DP %d\n", firstPart, secondPart);
//
//	struct compressedVal value1;
//	memcpy(value1.data,0);
//	printf("\n size %ld\n", sizeof(value1.data));

//transform(content);
//	time = clock() - time;
//	//	printf("Time taken: %f seconds\n", ((double)time)/CLOCKS_PER_SEC);
//	free(content);
//}

// float* getData(char *filePath, int *dataLength) {
// 	FILE *contentFile = fopen(filePath, "r");
// 	float *content = calloc(150*150*90, sizeof(float));
// 	int i = 0;
// 	while(fscanf(contentFile, "%f", &content[i]) == 1) {
// 		i++;
// 	}
// 	fclose(contentFile);
// 	*dataLength = i;
// 	return content;
// }

// struct compressedVal* getCompressedData(char *filePath, unsigned int magBits, unsigned int precBits) {
// 	int dataLength;
// 	float *uncompressedData = getData(filePath, &dataLength);
// 	struct compressedVal *compressedData = calloc(dataLength, sizeof(struct compressedVal));
// 	char sign;
// 	float firstPart, secondPart;
// 	uint32_t beforeDecimal, afterDecimal;
// 	int i;
// 	int offset;
// 	void *temp;
// 	char *bytes;

// 	for(i = 0; i < dataLength; i++) {
// 		sign = uncompressedData[i]>0?0:1;
// 		secondPart = modff(uncompressedData[i], &firstPart);
// 		beforeDecimal = (uint32_t) firstPart;
// 		afterDecimal = (uint32_t) (secondPart*10000);
// 		compressedData[i].data[0] = sign << 7;
// 		temp = &beforeDecimal;
// 		bytes = (char *) (temp);
// 		printf("asd %d %d\n", bytes[2], bytes[2] << 2);
// 		compressedData[i].data[0] = compressedData[i].data[0] | bytes[0] << 2;
// 		temp = &afterDecimal;
// 		bytes = (char *) (temp);
// 		compressedData[i].data[0] = compressedData[i].data[0] | bytes[3]; 
// 		compressedData[i].data[1] = bytes[1];
// 		compressedData[i].data[2] = bytes[0];
// 		printf("start %d %d\n", beforeDecimal, afterDecimal);
// 		printf("after %d:%d:%d\n\n", compressedData[i].data[0], compressedData[i].data[1], compressedData[i].data[2]);
// 	}
// 	free(uncompressedData);
// 	return compressedData;
// }

// int main(int argc, char *argv[]) {
// 	if(argc < 2) {
// 		printf("Missing additional params, recieved: %d\n", argc);
// 		return -1;
// 	}
// 	printf("Getting files in %s\n", argv[1]);
// 	//Grab absolute path of all relevant files
// 	char *files[10];
// 	getAbsFilenames(argv[1], files, ".txt.clean");
	
// 	int i = 0;
// 	printf("Found files..\n");
// 	for(i = 0; i < 6; i++) {
// 		printf("%d : %s\n", i+1, files[i]);
// 	}
// 	//get compressed data array with 5 bits for magnitude and 18 for precision
// 	getCompressedData(files[0], 5, 18);
// 	return 0;
// }

	//struct to represent basic file stats
	struct fileStats {
		int uncompressedCount;
		int runlengthCount;
	 	float maxVal;
	 	float minVal;
	 	float avgVal;
 	};




	int main() {
		char *directory = "../data/simulation_datasets/";
		char *files[100];
		int numFiles = 0;
		getAbsoluteFilepaths(files, directory, ".txt.clean", &numFiles);	//grab list of simulation datafiles

		int i;
		struct fileStats *stats = malloc(numFiles * sizeof(struct fileStats));
		float **datasets = malloc(numFiles*sizeof(float *));

		for(i = 0; i < numFiles; i++) {
			struct fileStats entry = { .maxVal = 0.0, .minVal = 0.0, .avgVal = 0.0, .uncompressedCount = 0, .runlengthCount = 0};
			stats[i] = entry;
			datasets[i] = getData(files[i], &stats[i].uncompressedCount, &stats[i].maxVal, &stats[i].minVal, &stats[i].avgVal);
			printf("Basic stats for file: %s\nNumber of values: %d, Max value: %f, Min value: %f, Average value: %f\n", files[i], stats[i].uncompressedCount, stats[i].maxVal, stats[i].minVal, stats[i].avgVal);
			printf("\tUncompressed size: %lu bytes\n", stats[i].uncompressedCount * sizeof(float));
			printf("Stats after runlength compression\n");
			struct runlengthEntry *runlengthCompressed = getRunlengthCompressedData(datasets[i], stats[i].uncompressedCount, &stats[i].runlengthCount);
			printf("\tNumber of runlength entries: %d, Runlength compressed size: %lu bytes\n\n", stats[i].runlengthCount, stats[i].runlengthCount * sizeof(struct runlengthEntry));
			printf("Stats after ZFP compression\n");
			size_t zfpCompressedSize = zfpCompress(datasets[i], 150, 150, 90, 0.00, 0);
			printf("\tZFP compressed size: %lu bytes\n\n", zfpCompressedSize);




			printf("Stats after 24 bit compression\n");

			printf("Stats for non byte aligned compression\n");

			//have printouts for loss of precisino values
			//grab data
			//get data sets
			//do runlength compression
			//do 24 bit compression
			//do non byte aligned compression
		}
		printf("done");


		//grab all files data
		//for each file
		//perform runlength encoding, write out new sizes
		//perform 24 bit compression, write out new sizes
		//perform 24 bit non aligned compression


		//compression performance
		//compress data?
		//run transformation
		return 0;
	}
