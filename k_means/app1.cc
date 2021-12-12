#include <stdio.h>

#include <iostream>

#include "psu_mr.h"

using namespace std;

static const char *inputFile = "input3.txt";
static const char *intermediateFile = "intermediate3.txt";
static const char *outputFile = "output3.txt";

int main(int argc, char* argv[]) {
	if (argc < 3) {
		cout << "Execution format- ./app1 <process_num> <total_num_processes>" << endl;
		return 0;
	}
	
	int process_num = atoi(argv[1]);
	int total_processes_num = atoi(argv[2]);

	psu_mr_setup(process_num, total_processes_num);

	psu_mr_map(&map_kmeans, (void *) inputFile, (void *) intermediateFile);

	psu_mr_reduce(&reduce_kmeans, (void *) intermediateFile, (void *) outputFile);
	
	psu_mr_destroy();

	return 0;
}
