#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "psu_dsm.h"
#include "psu_mutex.h"

using namespace std;
// #define COUNT 4*4096
#define COUNT 1024
int global_array[COUNT] __attribute__ ((aligned (4096)));
int partition_num[1024] __attribute__ ((aligned (4096)));

int logbase2(int n);
int get_partition_num(int id, int level, int num);
void partial_sort(int process_num, int total_processes_num);
void merge(int process_num, int total_processes_num);

void wait_partition(int a)
{
	psu_mutex_lock(0);
	partition_num[a]++;
	psu_mutex_unlock(0);

#if DEBUG
	cout << "waiting on partition num = " << a << endl;
#endif
	while(partition_num[a] < 2);
#if DEBUG
	cout << "done waiting on partition num = " << a << endl;
#endif

	return;
}

void initialize()
{
	srand((unsigned) time(NULL));
	for(int i = 0; i<COUNT; i++)
	{
		global_array[i] = rand()%500;
	}

}
int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		printf("Format is <sort> <Process_num> <total_num_Proceses>\n");
		return 0;
	}

#if DEBUG
	/*
	int iter;
	for (iter = 0; iter < COUNT - 1; iter++) {
		cout << (uint64_t) &global_array[iter] << "\t";
	}
	cout << (uint64_t) &global_array[iter] << endl;
	*/
#endif

	bool status = psu_start_lock();
	if (!status) {
		return 0;
	}
	psu_dsm_register_datasegment(&global_array, COUNT*sizeof(int)+(1024*sizeof(int)));
	psu_init_lock(0);
	int process_num;
	int total_processes_num;
	
	process_num = atoi(argv[1]);
	total_processes_num = atoi(argv[2]);

	if(process_num == 0)
	{
#if DEBUG
		cout << "process " << process_num << " initializing data..." << endl;
#endif
		initialize();
#if DEBUG
		cout << "process " << process_num << " initializing data done" << endl;
#endif
		psu_mutex_lock(0);
	 	partition_num[0]++;
		psu_mutex_unlock(0);
	}
	else
	{
#if DEBUG
		cout << "process " << process_num << " waiting for process 0 to initialize data..." << endl;
#endif
		while(partition_num[0] < 1);
#if DEBUG
		cout << "process " << process_num << " done waiting for process 0 to initialize data" << endl;
#endif
	}

#if DEBUG
	int iter;
	cout << "Initialized data-" << endl;
	for (iter = 0; iter < COUNT - 1; iter++) {
		cout << global_array[iter] << "\t";
	}
	cout << global_array[iter] << endl;
#endif

#if DEBUG
		cout << "process " << process_num << " starting partial sort..." << endl;
#endif
	partial_sort(process_num, total_processes_num);
#if DEBUG
		cout << "process " << process_num << " finished partial sort" << endl;
#endif

	//Do merging based on the process_num

	int p = process_num;
	int n = total_processes_num;
	//Do merging based on the lock for 2 processes
	int i = 0;
	// int i = 1;
	while((1<<i) < n)
	{
		if(p % (1<<i) == 0)
		{
#if DEBUG
			cout << "i = " << i << ", (1 << i) = " << (1 << i) << endl; 
			cout << "process " << p << " merging with arguments (" << (p / (1 << i)) << ", " << (n / (1 << i)) << ")..." << endl;
			merge(p/(1<<i),n/(1<<i));
			cout << "process " << p << " done merging" << endl;
#endif
			int b_id = get_partition_num(p,i,n);
#if DEBUG
			cout << "partition num = " << b_id << endl;
#endif
			wait_partition(b_id);
		}
		++i;
	}

	if(process_num == 0)
	{
#if DEBUG
		cout << "process 0 merging with arguments (0, 1)" << endl;
#endif
		merge(0,1);
#if DEBUG
		cout << "process 0 merge done" << endl;
#endif
		fstream outputFile;
		outputFile.open("output.txt", fstream::out | fstream::trunc);
		for(int i = 0; i<COUNT; i++) {
			//std::cout<<global_array[i]<<"\t";
			outputFile << global_array[i] << "\t";
		}
		outputFile.close();
	}
	psu_dsm_free();
	psu_stop_lock();
	return 0;
}

void partial_sort(int process_num, int total_processes_num)
{
	//choose the offset based on the process_num and total_processes_num
	int offset = process_num * (COUNT/total_processes_num);
	int size = COUNT/total_processes_num;
	int temp;

#if DEBUG
	cout << "partial sort size = " << size << endl;
#endif

	for (int i = 0; i < size -1 ; i++)
	{	
		for(int j = 0 ; j < size-i-1; j++)
		{
			if(global_array[j+offset] > global_array[j+1+offset])
			{
				temp = global_array[j+offset];
				global_array[j+offset] = global_array[j+1+offset];
				global_array[j+1+offset] = temp;
			}	
		}
	}
#if DEBUG
	cout << "offset = " << offset << endl;
	for (int i = offset; i < offset + size - 2; i++) {
		cout << "index = " << i << ", value = " << global_array[i] << "\t";
	}
	cout << "index = " << (offset + size - 1) << ", value = " << global_array[offset + size - 1] << endl;
#endif
	return;
}

void merge(int process_num, int total_processes_num)
{
	int offset = process_num * (COUNT/total_processes_num);
	int size = COUNT/total_processes_num/2;

	int* a = new int[size*2];
	int i = 0;
	int j = 0;
	int k = 0;
	while(i < size && j < size)
	{
		if(global_array[i+offset] < global_array[j+offset+size])
		{
			a[k++] = global_array[i+offset];
			i++;
		}
		else
		{
			a[k++] = global_array[j+offset+size];
			j++;
		}
	}

#if DEBUG
	cout << "One of the portions covered" << endl;
#endif

	if(j == size)
	{
		while(i<size)
		{
			a[k++] = global_array[i+offset];
			i++;
		}
		
	}
	else
	{	while(j<size)
		{
			a[k++] = global_array[j+offset+size];
			j++;
		}
	}

#if DEBUG
	cout << "Both of the portions covered, partial merge done" << endl;
#endif

	for(int i=0; i<size*2; ++i) {
		global_array[i+offset] = a[i];
#if DEBUG
		cout << "index = " << (i + offset) << ", value = " << global_array[i + offset] << "\t";
#endif
	}
#if DEBUG
	cout << endl;
#endif

	delete [] a;

}


int logbase2(int n)
{
	int i = 0;
	while(n>1)
	{
		i++;
		n=n/2;
	}
	return i;
}

int get_partition_num(int id, int level, int num)
{
	int k = logbase2(num)-level-1;
	int s_k = 1 << k;
	int idx = id/(1 << (level+1));
	return s_k + idx;
}
