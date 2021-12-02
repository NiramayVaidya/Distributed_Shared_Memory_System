// #include <bits/stdc++.h>

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

using namespace std;

int threadcount __attribute__ ((aligned (4096)));
int dummyvar __attribute__ ((aligned (4096)));

unsigned int g_tid;
unsigned int g_nthreads;

int numPoints = -1;
int numCentroids = -1;
vector<tuple<double, double>> initialCentroids;

static void barrier(bool dir) {
#if DEBUG
	cout << "in barrier" << endl;
	cout << "dir -> " << dir << endl;
#endif

	psu_mutex_lock(0);
	if (dir) {
		threadcount++;
	}
	else {
		threadcount--;
	}
	psu_mutex_unlock(0);

#if DEBUG
	cout << "waiting on threadcount to reach required value" << endl;
#endif

	if (dir) {
		while (threadcount != g_nthreads);
	}
	else {
		while (threadcount != 0);
	}
}

void psu_mr_setup(unsigned int tid, unsigned int nthreads) {
	g_tid = tid;
	g_nthreads = nthreads;
	psu_start_lock();
	psu_dsm_register_datasegment(&threadcount, PAGE_SIZE);
	psu_init_lock(0);
}

void psu_mr_map(void (*map_fp)(void *, void *), void *indata, void *outdata) {
#if DEBUG
	cout << "in psu_mr_map" << endl;
#endif

	string intermediateFile((const char *) outdata);
	fstream tempFile;
	tempFile.open(intermediateFile, fstream::out | fstream::trunc);
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
	psu_stop_lock();
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
	// cout << data[0] << endl;
	// cout << data[1];
	for (int i = 0; i < data.size(); i++) {
		cout << data[i] << endl;
	}
	// cout << endl;
#endif

	numPoints = stoi(data[0]);
	// data[1].erase(0, 1);
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

	psu_mutex_lock(0);
	fstream tempFile;
	tempFile.open(intermediateFile, fstream::out | fstream::app);
	for (int i = startIndex; i < endIndex; i++) {
		tempFile << get<0>(points[i]) << " " << get<1>(points[i]) << " " << closestCentroids[i - startIndex] << endl;
	}
	tempFile.close();
	psu_mutex_unlock(0);
	
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
	ifstream tempFile(intermediateFile);
	string line;
	while (getline(tempFile, line)) {
		data.push_back(line);
	}

	vector<tuple<double, double, int>> points;
	for (int i = 0; i < data.size(); i++) {
		if (points.size() != 0) {
			data[i].erase(0, 1);
		}
		int delimIndex = data[i].find(' ');
		double x = stod(data[i].substr(0, delimIndex));
		string partialData = data[i].substr(delimIndex + 1, data[i].size());
		delimIndex = partialData.find(' ');
		double y = stod(partialData.substr(0, delimIndex));
		int closestCentroid = stoi(partialData.substr(delimIndex + 1, partialData.size()));
		points.push_back(tuple<double, double, int>(x, y, closestCentroid));
	}
	// psu_mutex_unlock(0);

#if DEBUG
	cout << "points and closestCentroids -> " << endl;
	for (int i = 0; i < points.size(); i++) {
		cout << get<0>(points[i]) << " " << get<1>(points[i]) << " " << get<2>(points[2]) <<endl;
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

	psu_mutex_lock(0);
	fstream outFile;
	outFile.open(outputFile, fstream::out | fstream::app);
	outFile << setprecision(2) << fixed << avgX << " " << setprecision(2) << fixed << avgY << endl;
	outFile.close();
	psu_mutex_unlock(0);
	
	barrier(false);

	if (!g_tid) {
		ifstream oFile(outputFile);
		string line;
		while (getline(oFile, line)) {
			cout << line;
		}
		cout << endl;
	}
}
