#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

//assumes 0'd out files array
void getAbsFilenames(char* basedir, char* files[], char* extension) {
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

void displayStats(char* filePath) {
	FILE *contentFile;
	float min = 10000.0f;
	float max = -10000.0f;
	float currentVal;

	contentFile = fopen(filePath, "r");
	while(fscanf(contentFile, "%f", &currentVal) == 1) {
		if(currentVal > max)
			max = currentVal;
		if(currentVal < min)
			min = currentVal;
	}
	fclose(contentFile);
	printf("For file: %s\nMax: %f\nMin: %f\n", filePath, max, min);
}

int main(int argc, char* argv[]) {
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

	//displayStats(files[0]);
	//Get each files stats
	//for(i = 0; i < 6; i++) {
	//	displayStats(files[i]);
	//}


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
