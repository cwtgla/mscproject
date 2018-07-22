#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FILELOC "../data/stripped.txt"
#define ILEN 150
#define JLEN 150
#define KLEN 90

int F3D2C(
        unsigned int i_rng,unsigned int j_rng, // ranges, i.e. (hb-lb)+1
        int i_lb, int j_lb, int k_lb, // lower bounds
        int ix, int jx, int kx
        ) {
    return (i_rng*j_rng*(kx-k_lb)+i_rng*(jx-j_lb)+ix-i_lb);
}

//TODO function to take in pointer and give it files contents (cut out parts of main)

//TODO function to perform runlength encoding and see new ds size

//TODO function to perform zfs compression and see size

//TODO other function to encode and see size

int main(void) {
	float *content = malloc((ILEN*JLEN*KLEN)*sizeof(float));
	FILE *contentFile;

	contentFile = fopen(FILELOC,"r");

	int i = 0;
	while (fscanf(contentFile, "%f", &content[i]) == 1) {
		//printf("%f\n", content[i]);
		i++;
	}

	int j;
	int k;
	float reltmp;

	clock_t startTime, diff;
	startTime = clock();
	for(i=0;i<ILEN;i++) {
		for(j=0;j<JLEN;j++) {
			for(k=0;k<KLEN;k++) {
					/*reltmp = (
					 content[F3D2C(ilen,jlen,0,0,0,i+1,j,k)]
					+content[F3D2C(ilen,jlen,0,0,0,i-1,j,k)]
					+content[F3D2C(ilen,jlen,0,0,0,i,j+1,k)]
					+content[F3D2C(ilen,jlen,0,0,0,i,j-1,k)]
					+content[F3D2C(ilen,jlen,0,0,0,i,j,k+1)]
					+content[F3D2C(ilen,jlen,0,0,0,i,j,k-1)]
					)/6.0;
					//printf("sorting ref %d %d %d\n", i, j, k);
					content[F3D2C(ilen,jlen,0,0,0,i,j,k)] = content[F3D2C(ilen,jlen,0,0,0,i,j,k)]+reltmp;*/
			}
		}
	}
	diff = clock() - startTime;
	int msec = diff * 1000 / CLOCKS_PER_SEC;
	printf("Time taken %d seconds %d milliseconds", msec/1000, msec%1000);

	free(content);

	return 0;
}
