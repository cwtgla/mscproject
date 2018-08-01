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

int main(int argc, char* argv[]) {
	char *files[10];
	getAbsFilenames(argv[1], files, ".txt.clean");
	
//char*[] files = getAbsFilenames(argv[1]);
	int i = 0;
	for(i = 0; i < 6; i++) {
		printf("%s\n", files[i]);
	}

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
