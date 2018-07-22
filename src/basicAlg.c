#include <stdio.h>
#include <stdlib.h>

#define FILELOC "../data/stripped.txt"
#define EOF 1

int main(void) {
	FILE *contentFile;
	float *content = malloc((150*150*90)*sizeof(float));
	int i = 0;

	contentFile = fopen(FILELOC,"r");

	while (fscanf(contentFile, "%f", &content[i]) == EOF) {
		printf("%f\n", content[i]);
		i++;
	}

	free(content);

	return 0;
}
