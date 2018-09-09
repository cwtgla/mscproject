//FILE: tests.c 
//AUTHOR: Craig Thomson
//PURPOSE: Unit tests for compression/decompression/etc
 
#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "compressor.h"
#include <math.h>

/*
 * Purpose:
 * 		Test to confirm that getAbsoluteFilepaths() works as expected (produces correct number of files)
 */
MU_TEST(testGetAbsoluteFilepaths) {
	char *directory = "../data/test_datasets/getdata/";
	char *files[100];
	int fileCount;

	getAbsoluteFilepaths(files, directory, ".txt", &fileCount);
	mu_assert(fileCount == 2, "ERROR in testGetAbsoluteFilepaths: incorrect number of files found");
}

/*
 * Purpose:
 * 		Test to confirm that getData() is returning the expected amount of data and no lines are missed
 */
MU_TEST(testGetData) {
	char *file1 = "../data/test_datasets/getdata/100lines.txt";
	int lineCount1 = 0;
	float junk = 0.0; //junk used because we dont care about extracted max,min,mean
	float *contents1 = getData(file1, &lineCount1, &junk, &junk, &junk);
	mu_assert(lineCount1 == 100, "ERROR in testGetData: expected number of entries hasn't been retrieved (100)");
	free(contents1);

	char *file2 = "../data/test_datasets/getdata/0lines.txt";
	int lineCount2 = 0;
	float *contents2 = getData(file2, &lineCount2, &junk, &junk, &junk);
	mu_assert(lineCount2 == 0, "ERROR in testGetData: expected number of entries hasn't been retrieved (0)");
	free(contents2);
}

/*
 * Purpose:
 *		Test that the getRunlengthCompressedData() method correctly produces the compression ratio expected on custom datasets
 */
MU_TEST(testGetRunlengthCompressedDataset) {
	//dataset for a situation where there's no improvement in compression
	char *file = "../data/test_datasets/runlength/runlength_0_compression.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(file, &uncompressedCount, &junk, &junk, &junk);
	int compressedCount = 0;
	struct runlengthEntry *compressedData = getRunlengthCompressedData(uncompressedData, uncompressedCount, &compressedCount);
	mu_assert(uncompressedCount == compressedCount, "ERROR in testGetRunlengthCompressedData: 0 compression example isn't working as expected");
	free(uncompressedData);
	free(compressedData);

	//dataset for a situation where the number of data entries can be halved
	file = "../data/test_datasets/runlength/runlength_50_compression.txt";
	uncompressedCount = 0;
	uncompressedData = getData(file, &uncompressedCount, &junk, &junk, &junk);
	compressedCount = 0;
	compressedData = getRunlengthCompressedData(uncompressedData, uncompressedCount, &compressedCount);
	mu_assert(compressedCount == (uncompressedCount/2), "ERROR in testGetRunlengthCompressedData: 50% compression example isn't working as expected");
	free(uncompressedData);
	free(compressedData);

	//dataset for a situation where there's all data can be compressed into 1 entry
	file = "../data/test_datasets/runlength/runlength_100_compression.txt";
	uncompressedCount = 0;
	uncompressedData = getData(file, &uncompressedCount, &junk, &junk, &junk);
	compressedCount = 0;
	compressedData = getRunlengthCompressedData(uncompressedData, uncompressedCount, &compressedCount);
	mu_assert(compressedCount == 1, "ERROR in testGetRunlengthCompressedData: 100% compression example isn't working as expected");
	free(uncompressedData);
	free(compressedData);
}

/*
 * Purpose:
 *		Test that the getRunlengthDecompressedData() method decompresses and recreates original data as expected
 */
MU_TEST(testGetRunlengthDecompressedDataset) {
	//dataset for a situation where all data can be compressed into 1 entry
	char *file = "../data/test_datasets/runlength/runlength_100_compression.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(file, &uncompressedCount, &junk, &junk, &junk);

	int compressedCount = 0;
	struct runlengthEntry *compressedData = getRunlengthCompressedData(uncompressedData, uncompressedCount, &compressedCount);

	int decompressedCount = 0;
	float *decompressedData = getRunlengthDecompressedData(compressedData, compressedCount, &decompressedCount);

	mu_assert(uncompressedCount == decompressedCount,"ERROR in testGetRunlengthDecompressedData: before compression and decompressed size arent the same (100%)");
	
	int i;
	for(i = 0; i < decompressedCount; i++) {
		mu_assert(uncompressedData[i]==decompressedData[i], "ERROR in testGetRunlengthDecompressedData: before and after elements mismatch");
	}
	
	free(uncompressedData);
	free(compressedData);

	//dataset for a situation where number of entries can be halved
	file = "../data/test_datasets/runlength/runlength_50_compression.txt";
	uncompressedCount = 0;
	uncompressedData = getData(file, &uncompressedCount, &junk, &junk, &junk);
	compressedCount = 0;
	compressedData = getRunlengthCompressedData(uncompressedData, uncompressedCount, &compressedCount);
	decompressedCount = 0;
	decompressedData = getRunlengthDecompressedData(compressedData, compressedCount, &decompressedCount);
	mu_assert(uncompressedCount == decompressedCount,"ERROR in testGetRunlengthDecompressedData: before compression and decompressed size arent the same (50%)");

	for(i = 0; i < decompressedCount; i++) {
		mu_assert(uncompressedData[i]==decompressedData[i], "ERROR in testGetRunlengthDecompressedData: before and after elements mismatch");
	}
	free(uncompressedData);
	free(compressedData);
}

/*
 * Purpose:
 *		Test that the fixed 24 bit compression compresses as expected by comparing the values of the compressed bytes to expected values (done by hand)
 */
MU_TEST(testGet24BitCompressedDataset) {
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
		mu_assert(compressedData[i].data[2] == expectedData[j++], "ERROR in test24BitRateCompression (1-5-18): compressed bytes dont match expected");
		mu_assert(compressedData[i].data[1] == expectedData[j++], "ERROR in test24BitRateCompression (1-5-18): compressed bytes dont match expected");
		mu_assert(compressedData[i].data[0] == expectedData[j++], "ERROR in test24BitRateCompression (1-5-18): compressed bytes dont match expected");
	}
	free(compressedData);
	free(uncompressedData);


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
		mu_assert(compressedData[i].data[2] == expectedData[j++], "ERROR in test24BitRateCompression (1-7-16): compressed bytes dont match expected");
		mu_assert(compressedData[i].data[1] == expectedData[j++], "ERROR in test24BitRateCompression (1-7-16): compressed bytes dont match expected");
		mu_assert(compressedData[i].data[0] == expectedData[j++], "ERROR in test24BitRateCompression (1-7-16): compressed bytes dont match expected");
	}
	free(compressedData);
	free(uncompressedData);


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
		mu_assert(compressedData[i].data[2] == expectedData[j++], "ERROR in test24BitRateCompression (1-13-10): compressed bytes dont match expected");
		mu_assert(compressedData[i].data[1] == expectedData[j++], "ERROR in test24BitRateCompression (1-13-10): compressed bytes dont match expected");
		mu_assert(compressedData[i].data[0] == expectedData[j++], "ERROR in test24BitRateCompression (1-13-10): compressed bytes dont match expected");
	}
	free(compressedData);
	free(uncompressedData);


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
		mu_assert(compressedData[i].data[2] == expectedData[j++], "ERROR in test24BitRateCompression (1-20-3): compressed bytes dont match expected");
		mu_assert(compressedData[i].data[1] == expectedData[j++], "ERROR in test24BitRateCompression (1-20-3): compressed bytes dont match expected");
		mu_assert(compressedData[i].data[0] == expectedData[j++], "ERROR in test24BitRateCompression (1-20-3) compressed bytes dont match expected");
	}
	free(compressedData);
	free(uncompressedData);


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
		mu_assert(compressedData[i].data[2] == expectedData[j++], "ERROR in test24BitRateCompression (1-15-8) compressed bytes dont match expected");
		mu_assert(compressedData[i].data[1] == expectedData[j++], "ERROR in test24BitRateCompression (1-15-8) compressed bytes dont match expected");
		mu_assert(compressedData[i].data[0] == expectedData[j++], "ERROR in test24BitRateCompression (1-15-8) compressed bytes dont match expected");
	}
	free(compressedData);
	free(uncompressedData);
}

/*
 * Purpose:
 *		Test that the fixed 24 bit compression values before compression and after decompression are the same
 */
MU_TEST(testGet24BitDecompressedDataset) {
	//tests for 1-5-18 dataset
	char *testDataset = "../data/test_datasets/24bit/5lines_5mag_18prec.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	struct compressedVal *compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 5, 18);
	float *decompressedData = get24BitDecompressedData(compressedData, uncompressedCount, 5, 18);
	int i;

	for(i = 0; i < uncompressedCount; i++) {
		mu_assert(uncompressedData[i] == decompressedData[i], "ERROR in test24BitRateDecompression (1-5-18): compressed bytes dont match expected");
	}


	//tests for 1-7-16 dataset
	testDataset = "../data/test_datasets/24bit/5lines_7mag_16prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 7, 16);
	decompressedData = get24BitDecompressedData(compressedData, uncompressedCount, 7, 16);
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert(uncompressedData[i] == decompressedData[i], "ERROR in test24BitRateDecompression (1-7-16): compressed bytes dont match expected");
	}


	//tests for 1-13-10 dataset
	testDataset = "../data/test_datasets/24bit/5lines_13mag_10prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 13, 10);
	decompressedData = get24BitDecompressedData(compressedData, uncompressedCount, 13, 10);
	for(i = 0; i < uncompressedCount; i++) { 
		//rounding error happens here for some reason
		mu_assert(0.25 > fabs(uncompressedData[i] - decompressedData[i]), "ERROR in test24BitRateDecompression (1-13-10): compressed bytes dont match expected");
	}


	//tests for 1-20-3 dataset
	testDataset = "../data/test_datasets/24bit/5lines_20mag_3prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 20, 3);
	decompressedData = get24BitDecompressedData(compressedData, uncompressedCount, 20, 3);
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert(1/pow(10,numberOfDigits(3)-1) > fabs(uncompressedData[i] - decompressedData[i]), "ERROR in test24BitRateDecompression (1-20-3): compressed bytes dont match expected");
	}


	//tests for 1-15-8 dataset
	testDataset = "../data/test_datasets/24bit/5lines_15mag_8prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 15, 8);
	decompressedData = get24BitDecompressedData(compressedData, uncompressedCount, 15, 8);
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert(uncompressedData[i] == decompressedData[i], "ERROR in test24BitRateDecompression (1-15-8): compressed bytes dont match expected");
	}
}

/*
 * Purpose:
 *		Test that the fixed 24 bit compression values can be individually decompressed to their original values
 */
MU_TEST(testGetSingle24BitValueAndDecompress) {
	//tests for 1-5-18 dataset
	char *testDataset = "../data/test_datasets/24bit/5lines_5mag_18prec.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	struct compressedVal *compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 5, 18);
	int i;
	float individualValue = 0;
	for(i = 0; i < uncompressedCount; i++) {
		individualValue = getSingle24BitValue(compressedData, i, 5, 18);
		mu_assert(uncompressedData[i] == individualValue, "ERROR in testIndividual24BitCompressAndDecompress (1-5-18): Individual decompressed value not working as expected");
	}
	free(uncompressedData);
	free(compressedData);


	//tests for 1-7-16 dataset
	testDataset = "../data/test_datasets/24bit/5lines_7mag_16prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 7, 16);
	for(i = 0; i < uncompressedCount; i++) {
		individualValue = getSingle24BitValue(compressedData, i, 7, 16);
		mu_assert(uncompressedData[i] == individualValue, "ERROR in testIndividual24BitCompressAndDecompress (1-7-16): Individual decompressed value not working as expected");
	}
	free(uncompressedData);
	free(compressedData);


	//tests for 1-13-10 dataset
	testDataset = "../data/test_datasets/24bit/5lines_13mag_10prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 13, 10);
	for(i = 0; i < uncompressedCount; i++) {
		individualValue = getSingle24BitValue(compressedData, i, 13, 10);
		//deal with some precision being lost
		mu_assert(.25 > fabs(uncompressedData[i] - individualValue), "ERROR in test24BitRateDecompression (1-13-10): compressed bytes dont match expected");
	}
	free(uncompressedData);
	free(compressedData);


	//tests for 1-20-3 dataset
	testDataset = "../data/test_datasets/24bit/5lines_20mag_3prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 20, 3);
	for(i = 0; i < uncompressedCount; i++) {
		individualValue = getSingle24BitValue(compressedData, i, 20, 3);
		//deal with some precision being lost
		mu_assert(1/pow(10,numberOfDigits(3)-1) > fabs(uncompressedData[i] - individualValue), "ERROR in testIndividual24BitCompressAndDecompress (1-20-3): Individual decompressed value not working as expected");
	}
	free(uncompressedData);
	free(compressedData);


	//tests for 1-15-8 dataset
	testDataset = "../data/test_datasets/24bit/5lines_15mag_8prec.txt";
	uncompressedCount = 0;
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 15, 8);
	for(i = 0; i < uncompressedCount; i++) {
		individualValue = getSingle24BitValue(compressedData, i, 15, 8);
		mu_assert(uncompressedData[i] == individualValue, "ERROR in testIndividual24BitCompressAndDecompress (1-15-8): Individual decompressed value not working as expected");
	}
	free(uncompressedData);
	free(compressedData);
}

/*
 * Purpose:
 *		Test that a single fixed 24 bit compression value can be updated successfully. it does this by copying the 0th index float to the 4th index and comparing individual byte values
 */
MU_TEST(testInsertSingle24BitValueAndCompress) {
	//5 mag 18 prec dataset
	//Grab data from file, compress it
	char *testDataset = "../data/test_datasets/24bit/5lines_5mag_18prec.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	struct compressedVal *compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 5, 18);
	insertSingle24BitValue(compressedData, 31.15115, 4, 5, 18);
	mu_assert(compressedData[0].data[2] == compressedData[4].data[2], "ERROR in testSingle24BitCompress (1-5-18): Compressing and inserting new values not working as expected");
	mu_assert(compressedData[0].data[1] == compressedData[4].data[1], "ERROR in testSingle24BitCompress (1-5-18): Compressing and inserting new values not working as expected");
	mu_assert(compressedData[0].data[0] == compressedData[4].data[0], "ERROR in testSingle24BitCompress (1-5-18): Compressing and inserting new values not working as expected");
	free(uncompressedData);
	free(compressedData);


	//test for 1-7-16
	testDataset = "../data/test_datasets/24bit/5lines_7mag_16prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 7, 16);
	insertSingle24BitValue(compressedData, 127.1491, 4, 7, 16);
	mu_assert(compressedData[0].data[2] == compressedData[4].data[2], "ERROR in testSingle24BitCompress (1-7-16): Compressing and inserting new values not working as expected");
	mu_assert(compressedData[0].data[1] == compressedData[4].data[1], "ERROR in testSingle24BitCompress (1-7-16): Compressing and inserting new values not working as expected");
	mu_assert(compressedData[0].data[0] == compressedData[4].data[0], "ERROR in testSingle24BitCompress (1-7-16): Compressing and inserting new values not working as expected");
	free(uncompressedData);
	free(compressedData);


	//test for 1-13-10
	testDataset = "../data/test_datasets/24bit/5lines_13mag_10prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 13, 10);
	insertSingle24BitValue(compressedData, 8191.131, 4, 13, 10);
	mu_assert(compressedData[0].data[2] == compressedData[4].data[2], "ERROR in testSingle24BitCompress (1-13-10): Compressing and inserting new values not working as expected");
	mu_assert(compressedData[0].data[1] == compressedData[4].data[1], "ERROR in testSingle24BitCompress (1-13-10): Compressing and inserting new values not working as expected");
	mu_assert(compressedData[0].data[0] == compressedData[4].data[0], "ERROR in testSingle24BitCompress (1-13-10): Compressing and inserting new values not working as expected");
	free(uncompressedData);
	free(compressedData);


	//test for 1-15-8
	testDataset = "../data/test_datasets/24bit/5lines_15mag_8prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 15, 8);
	insertSingle24BitValue(compressedData, 32767.44, 4, 15, 8);
	mu_assert(compressedData[0].data[2] == compressedData[4].data[2], "ERROR in testSingle24BitCompress (1-15-8): Compressing and inserting new values not working as expected");
	mu_assert(compressedData[0].data[1] == compressedData[4].data[1], "ERROR in testSingle24BitCompress (1-15-8): Compressing and inserting new values not working as expected");
	mu_assert(compressedData[0].data[0] == compressedData[4].data[0], "ERROR in testSingle24BitCompress (1-15-8): Compressing and inserting new values not working as expected");
	free(uncompressedData);
	free(compressedData);


	//test for 1-7-16
	testDataset = "../data/test_datasets/24bit/5lines_7mag_16prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = get24BitCompressedData(uncompressedData, uncompressedCount, 7, 16);
	insertSingle24BitValue(compressedData, 127.1491, 4, 7, 16);
	mu_assert(compressedData[0].data[2] == compressedData[4].data[2], "ERROR in testSingle24BitCompress (1-7-16): Compressing and inserting new values not working as expected");
	mu_assert(compressedData[0].data[1] == compressedData[4].data[1], "ERROR in testSingle24BitCompress (1-7-16): Compressing and inserting new values not working as expected");
	mu_assert(compressedData[0].data[0] == compressedData[4].data[0], "ERROR in testSingle24BitCompress (1-7-16): Compressing and inserting new values not working as expected");
	free(uncompressedData);
	free(compressedData);
}

/*
 * Purpose:
 *		Test that the variable bit compression compresses as expected by comparing the values of the compressed bytes to expected values (done by hand)
 */
MU_TEST(testGetVariableBitCompressedDataset) {
	//4 mag 7 prec dataset
	char *testDataset = "../data/test_datasets/non_aligned/5lines_4mag_7prec.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	unsigned int compressedCount = 0;
	unsigned char *compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 4, 7);
	char *testCompressedVals = "../data/test_datasets/non_aligned/5lines_4mag_7prec_expected.txt";
	int expectedCount = 0;
	unsigned int *expectedData = getVerificationData(testCompressedVals, &expectedCount);
	int j = 0;
	int i;
	for(i = compressedCount-1; i >= 0; i--) {
		mu_assert(compressedData[i] == expectedData[j++], "ERROR in testGet24BitDecompressedData (1-4-7): compressed bytes dont match expected byte values");
	}


	//tests for 1-5-10 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_5mag_10prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 5, 10);
	testCompressedVals = "../data/test_datasets/non_aligned/5lines_5mag_10prec_expected.txt";
	expectedData = getVerificationData(testCompressedVals, &expectedCount);
	j = 0;
	for(i = compressedCount-1; i >= 0; i--) {
		mu_assert(compressedData[i] == expectedData[j++], "ERROR in testGet24BitDecompressedData (1-5-10): compressed bytes dont match expected byte values");
	}


	//tests for 1-7-12 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_7mag_12prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 7, 12);
	testCompressedVals = "../data/test_datasets/non_aligned/5lines_7mag_12prec_expected.txt";
	expectedData = getVerificationData(testCompressedVals, &expectedCount);
	j = 0;
	for(i = compressedCount-1; i >= 0; i--) {
		mu_assert(compressedData[i] == expectedData[j++], "ERROR in testGet24BitDecompressedData (1-7-12): compressed bytes dont match expected byte values");
	}


	//tests for 1-11-10 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_11mag_10prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 11, 10);
	testCompressedVals = "../data/test_datasets/non_aligned/5lines_11mag_10prec_expected.txt";
	expectedData = getVerificationData(testCompressedVals, &expectedCount);
	j = 0;
	for(i = compressedCount-1; i >= 0; i--) {
		mu_assert(compressedData[i] == expectedData[j++], "ERROR in testGet24BitDecompressedData (1-11-10): compressed bytes dont match expected byte values");
	}
}
 
/*
 * Purpose:
 *		Test that the variable bit compression values before compression and after decompression are roughly the same
 */
MU_TEST(testGetVariableBitDecompressedDataset) {
	//tests for 1-7-12 dataset
	char *testDataset = "../data/test_datasets/non_aligned/5lines_7mag_12prec.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	int i;
	int compressedCount = 0;
	unsigned char *compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 7, 12);
	int decompressedCount = 0;
	float *decompressedData = getVariableBitDecompressedData(compressedData, compressedCount, &decompressedCount, 7, 12);
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert(uncompressedData[i] == decompressedData[i], "ERROR in testGetVariableDecompressedData (1-5-18): Uncompressed and decompressed values dont match");
	}


	//tests for 1-5-10 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_5mag_10prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 5, 10);
	decompressedData = getVariableBitDecompressedData(compressedData, compressedCount, &decompressedCount, 5, 10);
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert(uncompressedData[i] == decompressedData[i], "ERROR in testGetVariableDecompressedData (1-5-10): Uncompressed and decompressed values dont match");
	}


	//tests for 1-4-7 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_4mag_7prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 4, 7);
	decompressedData = getVariableBitDecompressedData(compressedData, compressedCount, &decompressedCount, 4, 7);
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert(uncompressedData[i] == decompressedData[i], "ERROR in testGetVariableDecompressedData (1-4-7): Uncompressed and decompressed values dont match");
	}


	//tests for 1-11-10 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_11mag_10prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 11, 10);
	decompressedData = getVariableBitDecompressedData(compressedData, compressedCount, &decompressedCount, 11, 10);
	for(i = 0; i < uncompressedCount; i++) {
		mu_assert(uncompressedData[i] == decompressedData[i], "ERROR in testGetVariableDecompressedData (1-11-10): Uncompressed and decompressed values dont match");
	}
}

/*
 * Purpose:
 *		Test that once a variable bit compressed dataset can correctly retrieve and decompress individual values
 *
 */
MU_TEST(testGetSingleVariableBitValueAndDecompress) {
	//tests for 1-4-7 dataset
	char *testDataset = "../data/test_datasets/non_aligned/5lines_4mag_7prec.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	int i;
	int compressedCount = 0;
	float extractedVal;
	unsigned char *compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 4, 7);
	for(i = 0; i < uncompressedCount; i++) {
		extractedVal = getSingleVariableBitValue(compressedData, compressedCount, i, 4, 7);
		mu_assert(uncompressedData[i] == extractedVal, "ERROR IN testGetVariableBitDecompressedValue (1-4-7): Individual value isnt being retrieved and decompressed as expected");
	}


	//tests for 1-11-10 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_11mag_10prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 11, 10);
	for(i = 0; i < uncompressedCount; i++) {
		extractedVal = getSingleVariableBitValue(compressedData, compressedCount, i, 11, 10);
		mu_assert(uncompressedData[i] == extractedVal, "ERROR IN testGetVariableBitDecompressedValue (1-11-10): Individual value isnt being retrieved and decompressed as expected");
	}


	//tests for 1-5-10 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_5mag_10prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 5, 10);
	for(i = 0; i < uncompressedCount; i++) {
		extractedVal = getSingleVariableBitValue(compressedData, compressedCount, i, 5, 10);
		mu_assert(uncompressedData[i] == extractedVal, "ERROR IN testGetVariableBitDecompressedValue (1-5-10): Individual value isnt being retrieved and decompressed as expected");
	}


	//tests for 1-7-12 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_7mag_12prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 7, 12);
	for(i = 0; i < uncompressedCount; i++) {
		extractedVal = getSingleVariableBitValue(compressedData, compressedCount, i, 7, 12);
		mu_assert(uncompressedData[i] == extractedVal, "ERROR IN testGetVariableBitDecompressedValue (1-7-12): Individual value isnt being retrieved and decompressed as expected");
	}
}

/*
 * Purpose:
 *		Test that the function for inserting a new value into a variable bit compressed dataset works as expected
 */
MU_TEST(testInsertSingleVariableBitValueAndCompress) {
	//tests for 1-4-7 dataset
	char *testDataset = "../data/test_datasets/non_aligned/5lines_4mag_7prec.txt";
	int uncompressedCount = 0;
	float junk = 0.0;
	float *uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	int i;
	int compressedCount = 0;
	float extractedVal;
	unsigned char *compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 4, 7);
	for(i = 0; i < 4; i++) {
		//grab value 1 index ahead and move it to current index and verify the move was successful
		extractedVal = getSingleVariableBitValue(compressedData, compressedCount, i+1, 4, 7); //grab value 1 ahead
		insertSingleVariableBitValue(compressedData, compressedCount, i, extractedVal, 4, 7);
		mu_assert(extractedVal == getSingleVariableBitValue(compressedData, compressedCount, i, 4, 7), "ERROR IN testInsertVariableBitValue (1-4-7): Mismatch between inserted value and the value retrived after insertion");
	}
	free(uncompressedData);
	free(compressedData);


	//tests for 1-11-10 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_11mag_10prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 11, 10);
	for(i = 0; i < 4; i++) {
		//grab value 1 index ahead and move it to current index and verify the move was successful
		extractedVal = getSingleVariableBitValue(compressedData, compressedCount, i+1, 11, 10); //grab value 1 ahead
		insertSingleVariableBitValue(compressedData, compressedCount, i, extractedVal, 11, 10);
		mu_assert(extractedVal == getSingleVariableBitValue(compressedData, compressedCount, i, 11, 10), "ERROR IN testInsertVariableBitValue (1-11-10): Mismatch between inserted value and the value retrived after insertion");
	}
	free(uncompressedData);
	free(compressedData);


	//tests for 1-5-10 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_5mag_10prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 5, 10);
	for(i = 0; i < 4; i++) {
		//grab value 1 index ahead and move it to current index and verify the move was successful
		extractedVal = getSingleVariableBitValue(compressedData, compressedCount, i+1, 5, 10); //grab value 1 ahead
		insertSingleVariableBitValue(compressedData, compressedCount, i, extractedVal, 5, 10);
		mu_assert(extractedVal == getSingleVariableBitValue(compressedData, compressedCount, i, 5, 10), "ERROR IN testInsertVariableBitValue (1-5-10): Mismatch between inserted value and the value retrived after insertion");
	}
	free(uncompressedData);
	free(compressedData);


	//tests for 1-7-12 dataset
	testDataset = "../data/test_datasets/non_aligned/5lines_7mag_12prec.txt";
	uncompressedData = getData(testDataset, &uncompressedCount, &junk, &junk, &junk);
	compressedData = getVariableBitCompressedData(uncompressedData, uncompressedCount, &compressedCount, 7, 12);
	for(i = 0; i < 4; i++) {
		//grab value 1 index ahead and move it to current index and verify the move was successful
		extractedVal = getSingleVariableBitValue(compressedData, compressedCount, i+1, 7, 12); //grab value 1 ahead
		insertSingleVariableBitValue(compressedData, compressedCount, i, extractedVal, 7, 12);
		mu_assert(extractedVal == getSingleVariableBitValue(compressedData, compressedCount, i, 7, 12), "ERROR IN testInsertVariableBitValue (1-7-12): Mismatch between inserted value and the value retrived after insertion");
	}
	free(uncompressedData);
	free(compressedData);
}

/*
 * Purpose:
 *	Causes all individual tests to be run...
 */
MU_TEST_SUITE(test_suite) {
	MU_RUN_TEST(testGetAbsoluteFilepaths);
	MU_RUN_TEST(testGetData);
	MU_RUN_TEST(testGetRunlengthCompressedDataset);
	MU_RUN_TEST(testGetRunlengthDecompressedDataset);
	MU_RUN_TEST(testGet24BitCompressedDataset);
	MU_RUN_TEST(testGet24BitDecompressedDataset);
	MU_RUN_TEST(testGetSingle24BitValueAndDecompress);
	MU_RUN_TEST(testInsertSingle24BitValueAndCompress);
	MU_RUN_TEST(testGetVariableBitCompressedDataset);
	MU_RUN_TEST(testGetVariableBitDecompressedDataset);
	MU_RUN_TEST(testGetSingleVariableBitValueAndDecompress);
	MU_RUN_TEST(testInsertSingleVariableBitValueAndCompress);
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return minunit_status;
}