#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define FILELOC "../data/simulation_data/one_half_sim.txt.clean"
//#define FILELOC "../data/simulation_data/complete_sim.txt.clean"
//#define FILELOC "../data/simulation_data/one_quarter_sim.txt.clean"
//#define FILELOC "../data/simulation_data/one_third_sim.txt.clean"
//#define FILELOC "../data/simulation_data/three_quarter_sim.txt.clean"
#define FILELOC "../data/simulation_data/two_third_sim.txt.clean"
#define LINECOUNT 2025000

struct entry {
	float value;
	unsigned int count;
};

/* Convert from (i,j,k) for a 3D array to flat array index
rng - ranges i.e. (hb-lb)+1
lb - lower bounds */
inline unsigned int F3D2C(unsigned int i_rng, unsigned int j_rng, int i_lb, int j_lb, int k_lb, int ix, int jx, int kx) {
	return (i_rng*j_rng*(kx-k_lb)+i_rng*(jx-j_lb)+ix+i_lb);
}

int main(void) {
	//read floating points into array
	FILE *contentFile;
	float *content = calloc(LINECOUNT, sizeof(float));
	contentFile = fopen(FILELOC, "r");

	//while not EOF
	int i = 0;
	while (fscanf(contentFile, "%f", &content[i]) == 1) {
		i++;
	}

	//for each index e.g. from index 0 to linecount-1	
	printf("%d\n", F3D2C(150,150,0,0,0,150,0,0)); //start of all
	printf("%d\n", F3D2C(150,150,0,0,0,1,0,0)); //last i ind
	printf("%d\n", F3D2C(150,150,0,0,0,0,1,0));
	printf("%d\n", F3D2C(150,150,0,0,0,0,0,1));

	//F3D2C iBounds, jBounds, iBase, jBase, kBase, iInd, jInd, kInd
	free(content);	

	return 0;
}
