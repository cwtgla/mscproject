#include <stdio.h>
#include <stdlib.h>

//Takes in option and path. analysis or compress and a path to either a folder or a file
int main(int argc, char *argv[]) {
	char *homedir = getenv("HOME");

	if (homedir != NULL) {
		printf("Home dir in enviroment\n");
		printf("%s\n", homedir);
	}
//	if(argc != 2) {
//		printf("Need at least 1 argument (base directory path)\n"); 
//		return -1;
//	}
	//Create base array for filepaths
//	char *filePaths[100];
//	int count;
//	getAbsoluteFilepaths(filePaths, argv[1], ".txt.clean", &count);

//	int i;
//	printf("%d\n", count);
//	for(i=0; i<count; i++) {
//		printf("%d %s\n", i, filePaths[i]);
//	}
	
	///get uncompressed data
//	int uncompressedSize = 0;
//	printf("passing %s\n", filePaths[0]);
//	float *uncompressedData = getData(filePaths[0], &uncompressedSize);
//	for(i = 0; i < uncompressedSize; i++) {
//		printf("%d %.8f\n", i, uncompressedData[i]);
//	}


//	int compressedSize;
//	printf("%d", uncompressedSize);
//	struct runlengthEntry *compressedData = runlengthCompression(uncompressedData, uncompressedSize, &compressedSize);
//	for(i=0; i<compressedSize;i++) {
//		printf("index: %d value: %f count: %d\n", i, compressedData[i].value, compressedData[i].valuecount);
///		//if(compressedData[i].valuecount > 1) {
//		//	printf("index: %d value: %f count: %d\n", i, compressedData[i].value, compressedData[i].valuecount);
		//}
//	}

//}P
}