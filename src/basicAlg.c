#include <stdio.h>

#define FILELOC "../data/output.txt"

int main(void) {
	FILE *contentFile;
	float* content;	
	int contentSize = 150*150*90;
	
	*content = 0.14;
	contentFile = fopen(FILELOC,"r");

	while (fscanf(contentFile, "%f", content) == 1) {
		printf("%.6f\n", content);
	}
	//content = malloc(sizeof(float) * contentSize; 

	//free(content);

	return 0;
}
