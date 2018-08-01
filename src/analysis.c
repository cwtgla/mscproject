#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

struct entry {
	float value;
	uint count;
};

//pass in base dir of all data dumps as the only parameter
int main(int argc, char* argv[]) {
	DIR *dir = opendir(argv[1]); 




	//float *content = calloc(150*150*90, sizeof(float));
	//FILE *contentFile;	
	//contentFile = fopen(argv[1], "r");
	//int i = 0;
	//fscanf(contentFile, "%f", &content[i]);
	float min = content[i];
	float max = content[i];
	i++;	

	//while not EOF
	while (fscanf(contentFile, "%f", &content[i]) == 1) {
		if(content[i] > max)
			max = content[i];
		if(content[i] < min)
			min = content[i];
		i++;
	}
	free(content);
	printf("For file %s\nMax: %f\nMin:%f\n", argv[1], max, min);
	return 0;
}
