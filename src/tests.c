//FILE: Tests.c 
//AUTHOR: Craig Thomson
//Tests for compression
 
#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "complete_compressor.h"

int tests_run = 0;
 
/*
 * Purpose:
 * 		Test to confirm that getAbsoluteFilepaths() works as expected (produces correct number of files)
 *
 */
static char *testGetAbsoluteFilepaths() {
	char *directory = "/home/crags/Documents/compressor-repo/data/test_datasets/";
	char *files[100];
	int fileCount;

	getAbsoluteFilepaths(files, directory, ".txt", &fileCount);
	mu_assert("ERROR in testGetAbsoluteFilepaths: incorrect number of files found", fileCount == 3);
	return 0;
}

static char *testGetData() {
	char *file1 = "/home/crags/Documents/compressor-repo/data/test_datasets/100lines.txt";
	char *file2 = "/home/crags/Documents/compressor-repo/data/test_datasets/0lines.txt";
	int lineCount1 = 0;
	int lineCount2 = 0;

	float *contents1 = getData(file1, &lineCount1);
	mu_assert("ERROR in testGetData: expected number of entries hasn't been retrieved (100)", lineCount1 == 100);
	float *contents2 = getData(file2, &lineCount2);
	mu_assert("ERROR in testGetData: expected number of entries hasn't been retrieved (0)", lineCount2 == 0);
	free(contents1);
	free(contents2);

	return 0;
}

static char *testRunlengthCompression() {
	
}


 
static char * all_tests() {
	mu_run_test(testGetAbsoluteFilepaths);
	mu_run_test(testGetData);

	return 0;
}
 
int main(int argc, char **argv) {
	char *result = all_tests();
     	if (result != 0) {
	     printf("%s\n", result);
     	}
     	else {
	     printf("ALL TESTS PASSED\n");
	}
	printf("Tests run: %d\n", tests_run);
	return result != 0;
}
