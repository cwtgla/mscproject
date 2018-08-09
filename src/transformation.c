#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

float globalMax = -10000.0f;
float globalMin = 10000.0f;

struct rlEntry {
	float value;
	uint count;
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
				updateValue(i,j,k,&content);
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

void runlengthStats(float *values, int count) {
	struct rlEntry *entries = calloc(count, sizeof(struct rlEntry));
	entries[0].count = 1;
	entries[0].value = values[0];

	int i;
	int entryIndex = 0;
	for(i = 1; i < count; i++) {
		if(entries[entryIndex].value == values[i]) {
			entries[entryIndex].count++;
		} else {
			entryIndex++;
			entries[entryIndex].value = values[i];
			entries[entryIndex].count = 1;
		}
	}

	free(entries);
	printf("\t\t*****RUNLENGTH STATS *****\n");
	printf("\t\tNumber of indexes in runlength array %d, a net reduction in %d indexes\n\t\tSize in bytes of runlength array %lu bytes\n", entryIndex, count-entryIndex, sizeof(struct rlEntry)*entryIndex);
}

//Print stats from using zfp to compress values
void zfpStats(float *values, int count) {
	printf("\t\t\t*****ZFP STATS *****\n");
	size_t compressedSize;
//	compressedSize = compress(values, 150, 150, 90, 0.00, 0);
	printf("\t\t\tSize after zfp compression %lu bytes\n", compressedSize);
}

//Get basic file stats (max,min,mean and number of values)
void displayStats(char *filePath) {
	FILE *contentFile;

	contentFile = fopen(filePath, "r");
	float *content = calloc(150*150*90, sizeof(float));
	int i = 0;

	while(fscanf(contentFile, "%f", &content[i]) == 1) {
		i++;
	}
	fclose(contentFile);
	printf("Starting transformation!");
	clock_t time = clock();
	transform(content);
	time = clock() - time;
	printf("Time taken: %f seconds\n", ((double)time)/CLOCKS_PER_SEC);
	free(content);
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
	


	//printf("Getting stats for files..\n");
	//Get each files stats
	//for(i = 0; i < 6; i++) {
	displayStats(files[0]);
	//}

	//printf("Global stats..\n");
	//printf("Largest value found across files: %f. Smallest value found: %f\n", globalMax, globalMin);

	//struct dirent *directoryEntry;
	//DIR *directory = opendir(argv[1]);

	//if(directory == NULL) {
//		printf("ERROR");
//		return -1;
//	}
	
	//read entries
//	while((directoryEntry = readdir(directory)) != NULL) {
//		printf("%s\n", directoryEntry->d_name);
//	}
//	closedir(directory);
	return 0;
}
