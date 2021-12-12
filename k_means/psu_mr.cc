#include "psu_mr.h"
#include "psu_dsm.h"
#include "psu_mutex.h"

#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <map>
#include <tuple>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <climits>
#include <stdexcept>
#include <chrono>
#include <thread>

using namespace std;

int threadcount __attribute__ ((aligned (4096)));
int dummyvar __attribute__ ((aligned (4096)));

unsigned int g_tid;
unsigned int g_nthreads;

int numPoints = -1;
int numCentroids = -1;
vector<tuple<double, double>> initialCentroids;

string threadcountFile = "threadcount.txt";

static void barrier(bool dir) {
#if DEBUG
	cout << "in barrier" << endl;
	cout << "dir -> " << dir << endl;
#endif

	// psu_mutex_lock(0);
#if USE_DSM
	if (dir) {
		threadcount++;
	}
	else {
		threadcount--;
	}
#else
	ifstream tcFile(threadcountFile);
	string tc;
	while (getline(tcFile, tc));
	int threadCnt = stoi(tc);
#if DEBUG
	cout << "threadCnt -> " << threadCnt << endl;
#endif
	fstream tcf;
	tcf.open(threadcountFile, fstream::out | fstream::trunc);
	if (dir) {
		threadCnt++;
#if DEBUG
		cout << "threadCnt -> " << threadCnt << endl;
#endif
		tcf << threadCnt;
	}
	else {
		threadCnt--;
#if DEBUG
		cout << "threadCnt -> " << threadCnt << endl;
#endif
		tcf << threadCnt;
	}
	tcf.close();
#endif
	// psu_mutex_unlock(0);

#if DEBUG
	cout << "waiting on threadcount to reach required value" << endl;
#endif

#if USE_DSM
	if (dir) {
		while (threadcount != g_nthreads);
	}
	else {
		while (threadcount != 0);
	}
#else
	if (dir) {
		do {
			ifstream tcntFile(threadcountFile);
			while (getline(tcntFile, tc));
			try {
				threadCnt = stoi(tc);
			}
			catch (std::invalid_argument& e) {
				threadCnt = 0;
			}
		} while (threadCnt != g_nthreads);
	}
	else {
		do {
			ifstream tcntFile(threadcountFile);
			while (getline(tcntFile, tc));
			try {
				threadCnt = stoi(tc);
			}
			catch (std::invalid_argument& e) {
				threadCnt = g_nthreads;
			}
		} while (threadCnt != 0);
	}
#endif
}

void psu_mr_setup(unsigned int tid, unsigned int nthreads) {
	g_tid = tid;
	g_nthreads = nthreads;
	// psu_start_lock();
	psu_dsm_register_datasegment(&threadcount, PAGE_SIZE);
	// psu_init_lock(0);
#if USE_DSM == 0
	if (tid == 0) {
		fstream tcFile;
		tcFile.open(threadcountFile, fstream::out | fstream::trunc);
		tcFile << 0;
		tcFile.close();
	}
	/*
	ifstream tcntFile(threadcountFile);
	string line;
	while (getline(tcntFile, line));
	int count = stoi(line);
	if (count != 0) {
		fstream tcFile;
		tcFile.open(threadcountFile, fstream::out | fstream::trunc);
		tcFile << 0;
		tcFile.close();
	}
	*/
#endif
}

void psu_mr_map(void (*map_fp)(void *, void *), void *indata, void *outdata) {
#if DEBUG
	cout << "in psu_mr_map" << endl;
#endif

	string intermediateFile((const char *) outdata);
	fstream tempFile;
#if USE_MULTIPLE_INTERMEDIATE
	int delimPos = intermediateFile.find('.');
	/*
	for (int i = 0; i < g_nthreads; i++) {
		fstream tempFile;
		tempFile.open(intermediateFile.substr(0, delimPos) + to_string(i) + ".txt", fstream::out | fstream::trunc);
		tempFile.close();
	}
	*/
	tempFile.open(intermediateFile.substr(0, delimPos) + to_string(g_tid) + ".txt", fstream::out | fstream::trunc);
#else
	tempFile.open(intermediateFile, fstream::out | fstream::trunc);
#endif
	tempFile.close();

	map_fp(indata, outdata);
}

void psu_mr_reduce(void (*reduce_fp)(void *, void *), void *indata, void *outdata) {
#if DEBUG
	cout << "in psu_mr_reduce" << endl;
#endif

	string outputFile((const char *) outdata);
	fstream outFile;
	outFile.open(outputFile, fstream::out | fstream::trunc);
	outFile.close();

	reduce_fp(indata, outdata);
}

void psu_mr_destroy() {
	psu_dsm_free();
	// psu_stop_lock();
}

void map_kmeans(void *indata, void *outdata) {
#if DEBUG
	cout << "in map_kmeans" << endl;
#endif

	string inputFile((const char *) indata);
	string intermediateFile((const char *) outdata);

	vector<string> data;
	ifstream inFile(inputFile);
	string line;
	while (getline(inFile, line)) {
		data.push_back(line);
	}

#if DEBUG
	cout << "data ->" << endl;
	for (int i = 0; i < data.size(); i++) {
		cout << data[i] << endl;
	}
#endif

	numPoints = stoi(data[0]);
	numCentroids = stoi(data[1]);

#if DEBUG
	cout << "numPoints -> " << numPoints << endl;
	cout << "numCentroids -> " << numCentroids << endl;
#endif

	for (int i = 2 + numPoints; i < data.size(); i++) {
		data[i].erase(0, 1);
		int delimIndex = data[i].find(' ');
		initialCentroids.push_back(tuple<double, double>(stod(data[i].substr(0, delimIndex)), stod(data[i].substr(delimIndex + 1, data[i].size()))));
	}

#if DEBUG
	cout << "initialCentroids -> " << endl;
	for (int i = 0; i < initialCentroids.size(); i++) {
		cout << get<0>(initialCentroids[i]) << " " << get<1>(initialCentroids[i]) << endl;
	}
#endif

	vector<tuple<double, double>> points;
	for (int i = 2; i < 2 + numPoints; i++) {
		data[i].erase(0, 1);
		int delimIndex = data[i].find(' ');
		double x = stod(data[i].substr(0, delimIndex));
		double y = stod(data[i].substr(delimIndex + 1, data[i].size()));
		bool found = false;
		for (int j = 0; j < initialCentroids.size(); j++) {
			if (x == get<0>(initialCentroids[j]) && y == get<1>(initialCentroids[j])) {
				found = true;
				break;
			}
		}
		if (!found) {
			points.push_back(tuple<double, double>(x, y));
		}
	}

#if DEBUG
	cout << "points -> " << endl;
	for (int i = 0; i < points.size(); i++) {
		cout << get<0>(points[i]) << " " << get<1>(points[i]) << endl;
	}
#endif
	
	int startIndex = g_tid * (points.size() / g_nthreads);
	int endIndex = (g_tid + 1) * (points.size() / g_nthreads);

#if DEBUG
	cout << "startIndex -> " << startIndex << endl;
	cout << "endIndex -> " << endIndex << endl;
#endif

	vector<int> closestCentroids;
	for (int i = startIndex; i < endIndex; i++) {
		double leastDistance = numeric_limits<double>::max();
		int closestCentroid = -1;
		for (int j = 0; j < initialCentroids.size(); j++) {
			double distance = sqrt(pow(get<0>(points[i]) - get<0>(initialCentroids[j]), 2) + pow(get<1>(points[i]) - get<1>(initialCentroids[j]), 2));
			if (distance < leastDistance) {
				leastDistance = distance;
				closestCentroid = j;
			}
		}
		closestCentroids.push_back(closestCentroid);
	}

#if DEBUG
	cout << "closestCentroids ->" << endl;
	for (int i = startIndex; i < endIndex; i++) {
		cout << get<0>(points[i]) << " " << get<1>(points[i]) << " " << closestCentroids[i - startIndex] << endl;
	}
#endif

	// psu_mutex_lock(0);
	fstream tempFile;
#if USE_MULTIPLE_INTERMEDIATE
	int delimPos = intermediateFile.find('.');
	tempFile.open(intermediateFile.substr(0, delimPos) + to_string(g_tid) + ".txt", fstream::out | fstream::app);
#else
	tempFile.open(intermediateFile, fstream::out | fstream::app);
#endif
	for (int i = startIndex; i < endIndex; i++) {
		tempFile << get<0>(points[i]) << " " << get<1>(points[i]) << " " << closestCentroids[i - startIndex] << endl << flush;
#if DEBUG
#if USE_MULTIPLE_INTERMEDIATE
		cout << "flushing to the intermediate file " << intermediateFile.substr(0, delimPos) + to_string(g_tid) + ".txt -> " << get<0>(points[i]) << " " << get<1>(points[i]) << " " << closestCentroids[i - startIndex] << endl;
#else
		cout << "flushing to the intermediate file -> " << get<0>(points[i]) << " " << get<1>(points[i]) << " " << closestCentroids[i - startIndex] << endl;
#endif
#endif
		// flush(tempFile);
	}
	tempFile.close();
#if DEBUG
#if USE_MULTIPLE_INTERMEDIATE
	ifstream tFile(intermediateFile.substr(0, delimPos) + to_string(g_tid) + ".txt");
#else
	ifstream tFile(intermediateFile);
#endif
	int numLines = 0;
	while (getline(tFile, line)) {
		numLines++;
	}
	cout << "Updated num of lines in the intermediate file -> " << numLines << endl;
#endif
	// psu_mutex_unlock(0);
	
	barrier(true);
}

void reduce_kmeans(void *indata, void *outdata) {
#if DEBUG
	cout << "in reduce_kmeans" << endl;
#endif

	string intermediateFile((const char *) indata);
	string outputFile((const char *) outdata);

	// psu_mutex_lock(0);
	vector<string> data;
#if USE_MULTIPLE_INTERMEDIATE
	int delimPos = intermediateFile.find('.');
	for (int i = 0; i < g_nthreads; i++) {
		ifstream tempFile(intermediateFile.substr(0, delimPos) + to_string(i) + ".txt");
		string line;
		while (getline(tempFile, line)) {
			data.push_back(line);
		}
	}
#else
	ifstream tempFile(intermediateFile);
	string line;
	while (getline(tempFile, line)) {
		data.push_back(line);
	}
#endif

	vector<tuple<double, double, int>> points;
	for (int i = 0; i < data.size(); i++) {
		/*
		if (points.size() != 0) {
			data[i].erase(0, 1);
		}
		*/
		// cout << data[i] << endl;
		int delimIndex = data[i].find(' ');
		// cout << data[i].substr(0, delimIndex) << endl;
		double x = stod(data[i].substr(0, delimIndex));
		string partialData = data[i].substr(delimIndex + 1, data[i].size());
		delimIndex = partialData.find(' ');
		// cout << partialData.substr(0, delimIndex) << endl;
		double y = stod(partialData.substr(0, delimIndex));
		int closestCentroid = stoi(partialData.substr(delimIndex + 1, partialData.size()));
		points.push_back(tuple<double, double, int>(x, y, closestCentroid));
	}
	// psu_mutex_unlock(0);

#if DEBUG
	cout << "points and closestCentroids -> " << endl;
	for (int i = 0; i < points.size(); i++) {
		cout << get<0>(points[i]) << " " << get<1>(points[i]) << " " << get<2>(points[i]) <<endl;
	}
#endif

	double sumX = get<0>(initialCentroids[g_tid]);
	double sumY = get<1>(initialCentroids[g_tid]);
	int count = 1;
	for (int i = 0; i < points.size(); i++) {
		if (get<2>(points[i]) == g_tid) {
			sumX += get<0>(points[i]);
			sumY += get<1>(points[i]);
			count += 1;
		}
	}
	double avgX = sumX / count;
	double avgY = sumY / count;

#if DEBUG
	cout << "g_tid -> " << g_tid << endl;
	cout << "sumX -> " << sumX << endl;
	cout << "sumY -> " << sumY << endl;
	cout << "count -> " << count << endl;
	cout << setprecision(2) << fixed << avgX << " " << setprecision(2) << fixed << avgY << endl;
#endif

	this_thread::sleep_for(chrono::milliseconds(100));

	// psu_mutex_lock(0);
	fstream outFile;
	outFile.open(outputFile, fstream::out | fstream::app);
	outFile << setprecision(2) << fixed << avgX << " " << setprecision(2) << fixed << avgY << endl;
	outFile.close();
	// psu_mutex_unlock(0);
	
	// barrier(false);

	/*
	if (!g_tid) {
		cout << "Final centroids" << endl;
		ifstream oFile(outputFile);
		string line;
		while (getline(oFile, line)) {
			cout << line << endl;
		}
	}
	*/
}
