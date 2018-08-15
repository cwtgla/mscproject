#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

struct compressedVal {
	char data[3];
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

//assumes 0'd out files array
void getAbsFilenames(char *basedir, char *files[], char *extension) {
	struct dirent *directoryEntry;
	DIR *directory = opendir(basedir);
	int pathLen = strlen(basedir)+1; //for null char

	if(directory != NULL) {
		int count = 0;
		//whilst still iterating over an entry
		while((directoryEntry = readdir(directory)) != NULL) {
			//if the entry is of the desired extension
			if(strstr(directoryEntry->d_name,extension) != NULL) {
				files[count] = calloc((strlen(directoryEntry->d_name)+pathLen), sizeof(char));
				strcat(files[count], basedir);
				strcat(files[count], directoryEntry->d_name);
				count++;
			}
		}
	}
}

//Get basic file stats (max,min,mean and number of values)
void displayStats(char *filePath) {
	FILE *contentFile;
//	printf("After rounding:%f ", value);
	//printf("%X\n", &value);
	float fraction, intpart;

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
}

float* getData(char *filePath, int *dataLength) {
	FILE *contentFile = fopen(filePath, "r");
	float *content = calloc(150*150*90, sizeof(float));
	int i = 0;
	while(fscanf(contentFile, "%f", &content[i]) == 1) {
		i++;
	}
	fclose(contentFile);
	*dataLength = i;
	return content;
}

void getCompressedData(char *filePath, unsigned int magBits, unsigned int precBits) {
	int dataLength;
	float *uncompressedData = getData(filepath, &dataLength);
	struct compressedVal *compressedData = calloc(dataLength, sizeof(struct compressedVal));
	char sign;
	float firstPart, secondPart;
	uint32_t beforeDecimal, afterDecimal;
	int i;
	int offset;
	
	for(i = 0; i < dataLength; i++) {
		sign = uncompressedData[i]>0?1:0;
		secondPart = modff(uncompresedData[i], &firstPath);
		beforeDecimal = (uint32_t) firstPart;
		afterDecimal = (uint32_t) (secondPart*10000);
		compressedData[i].data[0] = sign << 7;
		compressedData[i].data[0] = beforeDecimal << 2 | compressedData[i].data[0];
		compressedData[i].data[0] = afterDecimal >> 16 | compressedData[i].data[0];
		
	}
	
	
	
	
	
	
	//If the fixed data size fits in a whole number of bytes
	if((1+magBits+precBits) % 8 == 0) {
		for(i=0;i<dataLength;i++) {
			offset=0;
			sign=uncompressedData[i]>0?1:0;
			secondPart = modff(uncompressedData[i], &firstPart);
			beforeDecimal = (uint32_t) firstPart;
			afterDecimal = (uint32_t) (secondHalf*10000); //TODO fix this based on precbits
			compressedData[i].data[0] = sign << 7; //Shifting sign in

			//If magnitude+sign fit in the first byte
			if(1+magBits <=8) {
				compressedData[i].data[0] = compressedData[i].data[0] | ((beforeDecimal >> (32 - (7-magBits))) & 0xFF);
				offset = 7-magBits;
			} else { //Magnitude overflows into another byte
				compressedData[i].data[0] = compressedData[i].data[0] | 
			}
		}
		//put in sign

		//
		//deal with magnitude
	} else {

	}


		compressedData[i].data[0] = sign << 7;

		//If magnitude and sign fit in the first byte
		if(magBits+1 <= 8) {
			compressedData[i].data[0] = compressedData[i].data[0] | (firstInt << (7-magBits));
			//If there's no space left in the byte
			if(magBits+1 == 8) {
				printf("unexpected case\n");
			} else {
				compressedData[i].data[0] = (secondInt >> (32-magBits)) & 0xFF;
			}

		} else {
			printf("unexpected case\n");
		}
	//	uncompressedVal[i].data = 
	}	
	
	
	//if(magBits+1 <=8) {
//
//		value1.data[0] = value1.data[0] | (firstPart << (7-magBits)); //
//	}

	//shift in magnitude
	//take magnitude currently in int
	//cast to void
	//cast to chars, shift the existing char into what i want
//	value1.data[0] = value1.data[0] | (firstPart << (7-magBits)); //this gets shifted 7-mag bits
	
//	printf("content %d %d %d\n", value1.data[0], value1.data[1], value1.data[2]);
	


	free(compressedData);
}

int main(int argc, char *argv[]) {
	if(argc < 2) {
		printf("Missing additional params, recieved: %d\n", argc);
		return -1;
	}
	printf("Getting files in %s\n", argv[1]);
	//Grab absolute path of all relevant files
	char *files[10];
	getAbsFilenames(argv[1], files, ".txt.clean");
	
	int i = 0;
	printf("Found files..\n");
	for(i = 0; i < 6; i++) {
		printf("%d : %s\n", i+1, files[i]);
	}
	//get compressed data array with 5 bits for magnitude and 18 for precision
	getCompressedData(files[0], 5, 18);
	return 0;
}
