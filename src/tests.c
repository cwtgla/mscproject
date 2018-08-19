//FILE: Tests.c 
//AUTHOR: Craig Thomson
//Tests for compression
 
#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "complete_compressor.h"

int tests_run = 0;
//char *homedir = getenv("HOME");

/*
 * Purpose:
 * 		Test to confirm that getAbsoluteFilepaths() works as expected (produces correct number of files)
 */
static char *testGetAbsoluteFilepaths() {
	char *directory = "/Users/Craig/Documents/compressor-repo/data/test_datasets/getdata/";
	char *files[100];
	int fileCount;

	getAbsoluteFilepaths(files, directory, ".txt", &fileCount);
	mu_assert("ERROR in testGetAbsoluteFilepaths: incorrect number of files found", fileCount == 2);
	return 0;
}

/*
 * Purpose:
 * 		Test to confirm that getData() is returning the expected amount of data and no lines are missed
 */
static char *testGetData() {
	char *file1 = "/Users/Craig/Documents/compressor-repo/data/test_datasets/getdata/100lines.txt";
	char *file2 = "/Users/Craig/Documents/compressor-repo/data/test_datasets/getdata/0lines.txt";
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

/*
 * Purpose:
 *		Test that the runlengthCompression() method correctly performs the compression ratio expected on custom datasets
 */
static char *testRunlengthCompression() {
	//dataset for a situation where there's no improvement in compression
	char *file1 = "/Users/Craig/Documents/compressor-repo/data/test_datasets/runlength/runlength_0_compression.txt";
	int uncompressedCount = 0;
	float *uncompressedData = getData(file1, &uncompressedCount);
	int compressedCount = 0;
	struct runlengthEntry *compressedData = runlengthCompression(uncompressedData, uncompressedCount, &compressedCount);
	mu_assert("ERROR in testRunlengthCompression: 0 compression example isn't working as expected", uncompressedCount == compressedCount);
	free(uncompressedData);
	free(compressedData);

	//dataset for a situation where the number of data entries can be halved
	char *file2 = "/Users/Craig/Documents/compressor-repo/data/test_datasets/runlength/runlength_50_compression.txt";
	uncompressedCount = 0;
	uncompressedData = getData(file2, &uncompressedCount);
	compressedCount = 0;
	compressedData = runlengthCompression(uncompressedData, uncompressedCount, &compressedCount);
	mu_assert("Error in testRunlengthCompression: 50% compression example isn't working as expected", compressedCount == (uncompressedCount/2));
	free(uncompressedData);
	free(compressedData);

	//dataset for a situation where there's all data can be compressed into 1 entry
	char *file3 = "/Users/Craig/Documents/compressor-repo/data/test_datasets/runlength/runlength_100_compression.txt";
	uncompressedCount = 0;
	uncompressedData = getData(file3, &uncompressedCount);
	compressedCount = 0;
	compressedData = runlengthCompression(uncompressedData, uncompressedCount, &compressedCount);
	mu_assert("Error in testRunlengthCompression: 100% compression example isn't working as expected", compressedCount == 1);
	free(uncompressedData);
	free(compressedData);

	return 0;
}

/*
 * Purpose:
 *		Test that the runlengthDecompression() method decompresses and recreates original data as expected
 */
static char *testRunlengthDecompression() {
	//dataset for a situation where there's all data can be compressed into 1 entry
	char *file1 = "/Users/Craig/Documents/compressor-repo/data/test_datasets/runlength/runlength_100_compression.txt";
	int uncompressedCount = 0;
	float *uncompressedData = getData(file1, &uncompressedCount);
	int compressedCount = 0;
	struct runlengthEntry *compressedData = runlengthCompression(uncompressedData, uncompressedCount, &compressedCount);
	int decompressedCount = 0;
	float *decompressedData = runlengthDecompression(compressedData, compressedCount, &decompressedCount);
	mu_assert("Error in testRunlengthDecompression: before compression and decompressed size arent the same", uncompressedCount == decompressedCount);
	int i;
	for(i = 0; i < decompressedCount; i++) {
		mu_assert("Error in testRunlengthDecompression: before and after elements mismatch", uncompressedData[i]==decompressedData[i]);
	}
	free(uncompressedData);
	free(compressedData);

	//dataset for a situation where there's all data can be compressed into 1 entry
	char *file2 = "/Users/Craig/Documents/compressor-repo/data/test_datasets/runlength/runlength_50_compression.txt";
	uncompressedCount = 0;
	uncompressedData = getData(file2, &uncompressedCount);
	compressedCount = 0;
	compressedData = runlengthCompression(uncompressedData, uncompressedCount, &compressedCount);
	decompressedCount = 0;
	decompressedData = runlengthDecompression(compressedData, compressedCount, &decompressedCount);
	mu_assert("Error in testRunlengthDecompression: before compression and decompressed size arent the same", uncompressedCount == decompressedCount);

	for(i = 0; i < decompressedCount; i++) {
		mu_assert("Error in testRunlengthDecompression: before and after elements mismatch", uncompressedData[i]==decompressedData[i]);
	}
	free(uncompressedData);
	free(compressedData);

	return 0;
}

static char *test24BitRateCompression() {
	char *file1 = "/Users/Craig/Documents/compressor-repo/data/test_datasets/24bit/10lines_5mag_18prec.txt";
	int uncompressedCount = 0;
	float *uncompressedData = getData(file1, &uncompressedCount);
	//float *compressedData = get24BitCompressedData
	return 0;
}
 
static char *all_tests() {
	mu_run_test(testGetAbsoluteFilepaths);
	mu_run_test(testGetData);
	mu_run_test(testRunlengthCompression);
	mu_run_test(testRunlengthDecompression);
	mu_run_test(test24BitRateCompression);

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
