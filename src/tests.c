//FILE: Tests.c 
//AUTHOR: Craig Thomson
//Tests for compression
 
#include <stdio.h>
#include "minunit.h"
#include "complete_compressor.h"

int tests_run = 0;
 
int foo = 7;
int bar = 4;

static char *test_read_data_correct_count() {
 	char *testDirName = "/home/crags/Documents/compressor-repo/data/test_datasets/";
	char *testFileName = "ds1.txt";

}

static char *testGetAbsoluteFilepaths() {
	char *directory = "/home/crags/Documents/compressor-repo/data/test_datasets/";
	char *files[100];
	int fileCount;

	getAbsoluteFilepaths(files, directory, ".txt.", &fileCount);
	mu_assert("ERROR in testGetAbsoluteFilepaths: incorrect number of files found", fileCount == 0);
	return 0;
}

static char * test_foo() {
	mu_assert("error, foo != 7", foo == 7);
	return 0;
}
 
static char * test_bar() {
	mu_assert("error, bar != 5", bar == 5);
	return 0;
}
 
static char * all_tests() {
     //mu_run_test(test_foo);
     //mu_run_test(test_bar);

	testGetAbsoluteFilepaths();
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
