//FILE: Tests.c 
//AUTHOR: Craig Thomson
//PURPOSE: Tests for compression/decompression/etc
 
#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "complete_compressor.h"
#include <math.h>

int tests_run = 0;
//char *homedir = getenv("HOME");

/*
 * Purpose:
 * 		Test to confirm that getAbsoluteFilepaths() works as expected (produces correct number of files)
 */
static char *testGetAbsoluteFilepaths() {
	char *directory = "../data/test_datasets/getdata/";
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
	char *file1 = "../data/test_datasets/getdata/100lines.txt";
	char *file2 = "../data/test_datasets/getdata/0lines.txt";
	int lineCount1 = 0;
	int lineCount2 = 0;
	float junk = 0.0;

	float *contents1 = getData(file1, &lineCount1, &junk, &junk, &junk);
	mu_assert("ERROR in testGetData: expected number of entries hasn't been retrieved (100)", lineCount1 == 100);
	float *contents2 = getData(file2, &lineCount2, &junk, &junk, &junk);
	mu_assert("ERROR in testGetData: expected number of entries hasn't been retrieved (0)", lineCount2 == 0);
	free(contents1);
	free(contents2);

	return 0;
}

/*
 * Purpose:
 *		Test that the runlengthCompression() method correctly performs the compression ratio expected on custom datasets
 */
static char *testGetRunlengthCompressedData() {
	//dataset for a situation where there's no improvement in compression
	char *file1 = "../data/test_datasets/runlength/runlength_0_compression.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(file1, &uncompressedCount, &junk, &junk, &junk);
	int compressedCount = 0;
	struct runlengthEntry *compressedData = getRunlengthCompressedData(uncompressedData, uncompressedCount, &compressedCount);
	mu_assert("ERROR in testRunlengthCompression: 0 compression example isn't working as expected", uncompressedCount == compressedCount);
	free(uncompressedData);
	free(compressedData);

	//dataset for a situation where the number of data entries can be halved
	char *file2 = "../data/test_datasets/runlength/runlength_50_compression.txt";
	uncompressedCount = 0;
	uncompressedData = getData(file2, &uncompressedCount, &junk, &junk, &junk);
	compressedCount = 0;
	compressedData = getRunlengthCompressedData(uncompressedData, uncompressedCount, &compressedCount);
	mu_assert("Error in testRunlengthCompression: 50% compression example isn't working as expected", compressedCount == (uncompressedCount/2));
	free(uncompressedData);
	free(compressedData);

	//dataset for a situation where there's all data can be compressed into 1 entry
	char *file3 = "../data/test_datasets/runlength/runlength_100_compression.txt";
	uncompressedCount = 0;
	uncompressedData = getData(file3, &uncompressedCount, &junk, &junk, &junk);
	compressedCount = 0;
	compressedData = getRunlengthCompressedData(uncompressedData, uncompressedCount, &compressedCount);
	mu_assert("Error in testRunlengthCompression: 100% compression example isn't working as expected", compressedCount == 1);
	free(uncompressedData);
	free(compressedData);

	return 0;
}

/*
 * Purpose:
 *		Test that the runlengthDecompression() method decompresses and recreates original data as expected
 */
static char *testGetRunlengthDecompressedData() {
	//dataset for a situation where there's all data can be compressed into 1 entry
	char *file1 = "../data/test_datasets/runlength/runlength_100_compression.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(file1, &uncompressedCount, &junk, &junk, &junk);
	int compressedCount = 0;
	struct runlengthEntry *compressedData = getRunlengthCompressedData(uncompressedData, uncompressedCount, &compressedCount);
	int decompressedCount = 0;
	float *decompressedData = getRunlengthDecompressedData(compressedData, compressedCount, &decompressedCount);
	mu_assert("Error in testRunlengthDecompression: before compression and decompressed size arent the same (100%)", uncompressedCount == decompressedCount);
	int i;
	for(i = 0; i < decompressedCount; i++) {
		mu_assert("Error in testRunlengthDecompression: before and after elements mismatch", uncompressedData[i]==decompressedData[i]);
	}
	free(uncompressedData);
	free(compressedData);

	//dataset for a situation where there's all data can be compressed into 1 entry
	char *file2 = "../data/test_datasets/runlength/runlength_50_compression.txt";
	uncompressedCount = 0;
	uncompressedData = getData(file2, &uncompressedCount, &junk, &junk, &junk);
	compressedCount = 0;
	compressedData = getRunlengthCompressedData(uncompressedData, uncompressedCount, &compressedCount);
	decompressedCount = 0;
	decompressedData = getRunlengthDecompressedData(compressedData, compressedCount, &decompressedCount);
	mu_assert("Error in testRunlengthDecompression: before compression and decompressed size arent the same (50%)", uncompressedCount == decompressedCount);

	for(i = 0; i < decompressedCount; i++) {
		mu_assert("Error in testRunlengthDecompression: before and after elements mismatch", uncompressedData[i]==decompressedData[i]);
	}
	free(uncompressedData);
	free(compressedData);

	return 0;
}

/*
 * Purpose:
 *		Test that the fixed 24 bit compression compresses as expected by comparing the values of the compressed bytes to expected values (done by hand)
 */
static char *testGet24BitCompressedData() {
	//5 mag 18 prec dataset
	//Grab data from file, compress it
	char *testDataset = "../data/test_datasets/24bit/5lines_5mag_18prec.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	struct compressedVal *compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 5, 18);
	//Grab expected int values for each byte (3*uncompressedCount values)
	char *testCompressedVals = "../data/test_datasets/24bit/5lines_5mag_18prec_expected.txt";
	int expectedCount = 0;
	int *expectedData = getVerificationData(testCompressedVals, &expectedCount);

	int i;
	int j = 0;
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert("Error in test24BitRateCompression (1-5-18): compressed bytes dont match expected", compressedData[i].data[2] == expectedData[j++]);
		mu_assert("Error in test24BitRateCompression (1-5-18): compressed bytes dont match expected", compressedData[i].data[1] == expectedData[j++]);
		mu_assert("Error in test24BitRateCompression (1-5-18): compressed bytes dont match expected", compressedData[i].data[0] == expectedData[j++]);
	}

	//7 mag 16 prec dataset
	//Grab data from file, compress it
	testDataset = "../data/test_datasets/24bit/5lines_7mag_16prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 7, 16);
	//Grab expected int values for each byte (3*uncompressedCount values)
	testCompressedVals = "../data/test_datasets/24bit/5lines_7mag_16prec_expected.txt";
	expectedCount = 0;
	expectedData = getVerificationData(testCompressedVals, &expectedCount);
	j = 0;

	for(i = 0; i < uncompressedCount; i++) {
		mu_assert("Error in test24BitRateCompression (1-7-16): compressed bytes dont match expected", compressedData[i].data[2] == expectedData[j++]);
		mu_assert("Error in test24BitRateCompression (1-7-16): compressed bytes dont match expected", compressedData[i].data[1] == expectedData[j++]);
		mu_assert("Error in test24BitRateCompression (1-7-16): compressed bytes dont match expected", compressedData[i].data[0] == expectedData[j++]);
	}

	//13 mag 10 prec dataset
	//Grab data from file, compress it
	testDataset = "../data/test_datasets/24bit/5lines_13mag_10prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 13, 10);
	//Grab expected int values for each byte (3*uncompressedCount values)
	testCompressedVals = "../data/test_datasets/24bit/5lines_13mag_10prec_expected.txt";
	expectedCount = 0;
	expectedData = getVerificationData(testCompressedVals, &expectedCount);
	j = 0;

	for(i = 0; i < uncompressedCount; i++) {
		mu_assert("Error in test24BitRateCompression (1-13-10): compressed bytes dont match expected", compressedData[i].data[2] == expectedData[j++]);
		mu_assert("Error in test24BitRateCompression (1-13-10): compressed bytes dont match expected", compressedData[i].data[1] == expectedData[j++]);
		mu_assert("Error in test24BitRateCompression (1-13-10): compressed bytes dont match expected", compressedData[i].data[0] == expectedData[j++]);
	}

	//20 mag 3 prec dataset
	//Grab data from file, compress it
	testDataset = "../data/test_datasets/24bit/5lines_20mag_3prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 20, 3);
	//Grab expected int values for each byte (3*uncompressedCount values)
	testCompressedVals = "../data/test_datasets/24bit/5lines_20mag_3prec_expected.txt";
	expectedCount = 0;
	expectedData = getVerificationData(testCompressedVals, &expectedCount);
	j = 0;

	for(i = 0; i < uncompressedCount; i++) {
		mu_assert("Error in test24BitRateCompression (1-20-3): compressed bytes dont match expected", compressedData[i].data[2] == expectedData[j++]);
		mu_assert("Error in test24BitRateCompression (1-20-3): compressed bytes dont match expected", compressedData[i].data[1] == expectedData[j++]);
		mu_assert("Error in test24BitRateCompression (1-20-3) compressed bytes dont match expected", compressedData[i].data[0] == expectedData[j++]);
	}

	//15 mag 8 prec dataset
	//Grab data from file, compress it
	testDataset = "../data/test_datasets/24bit/5lines_15mag_8prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 15, 8);
	//Grab expected int values for each byte (3*uncompressedCount values)
	testCompressedVals = "../data/test_datasets/24bit/5lines_15mag_8prec_expected.txt";
	expectedCount = 0;
	expectedData = getVerificationData(testCompressedVals, &expectedCount);
	j = 0;

	for(i = 0; i < uncompressedCount; i++) {
		mu_assert("Error in test24BitRateCompression (1-15-8): compressed bytes dont match expected", compressedData[i].data[2] == expectedData[j++]);
		mu_assert("Error in test24BitRateCompression (1-15-8): compressed bytes dont match expected", compressedData[i].data[1] == expectedData[j++]);
		mu_assert("Error in test24BitRateCompression (1-15-8) compressed bytes dont match expected", compressedData[i].data[0] == expectedData[j++]);
	}
	return 0;
}

/*
 * Purpose:
 *		Test that the fixed 24 bit compression values before compression and after decompression are roughly the same
 */
static char *testGet24BitDecompressedData() {
	//tests for 1-5-18 dataset
	char *testDataset = "../data/test_datasets/24bit/5lines_5mag_18prec.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	struct compressedVal *compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 5, 18);
	float *decompressedData = get24BitDecompressedData(compressedData, uncompressedCount, 5, 18);
	int i;
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert("Error in test24BitRateDecompression (1-5-18): compressed bytes dont match expected", pow(10,numDigits(18)) > fabs(uncompressedData[i] - decompressedData[i]));
	}

	//tests for 1-7-16 dataset
	testDataset = "../data/test_datasets/24bit/5lines_7mag_16prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 7, 16);
	decompressedData = get24BitDecompressedData(compressedData, uncompressedCount, 7, 16);
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert("Error in test24BitRateDecompression (1-7-16): compressed bytes dont match expected", pow(10,numDigits(16)) > fabs(uncompressedData[i] - decompressedData[i]));
	}

	//tests for 1-13-10 dataset
	testDataset = "../data/test_datasets/24bit/5lines_13mag_10prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 13, 10);
	decompressedData = get24BitDecompressedData(compressedData, uncompressedCount, 13, 10);
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert("Error in test24BitRateDecompression (1-13-10): compressed bytes dont match expected", pow(10,numDigits(10)) > fabs(uncompressedData[i] - decompressedData[i]));
	}

	//tests for 1-20-3 dataset
	testDataset = "../data/test_datasets/24bit/5lines_20mag_3prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 20, 3);
	decompressedData = get24BitDecompressedData(compressedData, uncompressedCount, 20, 3);
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert("Error in test24BitRateDecompression (1-20-3): compressed bytes dont match expected", pow(10,numDigits(3)) > fabs(uncompressedData[i] - decompressedData[i]));
	}

	//tests for 1-15-8 dataset
	testDataset = "../data/test_datasets/24bit/5lines_15mag_8prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 15, 8);
	decompressedData = get24BitDecompressedData(compressedData, uncompressedCount, 15, 8);
	for(i = 0; i < uncompressedCount; i++) {
		printf("val1 %f val2 %f\n", uncompressedData[i], decompressedData[i]);
		mu_assert("Error in test24BitRateDecompression (1-15-8): compressed bytes dont match expected", pow(10,numDigits(8)) > fabs(uncompressedData[i] - decompressedData[i]));
	}

	return 0;
}

 
static char *all_tests() {
	mu_run_test(testGetAbsoluteFilepaths);
	mu_run_test(testGetData);
	mu_run_test(testGetRunlengthCompressedData);
	mu_run_test(testGetRunlengthDecompressedData);
	mu_run_test(testGet24BitCompressedData);
	mu_run_test(testGet24BitDecompressedData);
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
