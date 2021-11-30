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

using namespace std;

int threadcount __attribute__ ((aligned (4096)));
int dummyvar __attribute__ ((aligned (4096)));

unsigned int g_tid;
unsigned int g_nthreads;

string intermediateFile;
string outputFile;

vector<string> inputLines;
int numInputLines = -1;
int startIndex = -1;
int endIndex = -1;

map<string, int> wordCount;
vector<tuple<string, int>> gWordCount;
int numInputWords;

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

void psu_mr_map(void *(*map_fp)(void *), void *indata, void *outdata) {
#if DEBUG
	cout << "in psu_mr_map" << endl;
#endif

	string inputFile((const char *) indata);
	string interFile((const char *) outdata);
	intermediateFile = interFile;
	fstream tempFile;
	tempFile.open(intermediateFile, fstream::out | fstream::trunc);
	tempFile.close();
	
	ifstream inFile(inputFile);
	string line;
	while (getline(inFile, line)) {
		inputLines.push_back(line);
	}
	numInputLines = inputLines.size();
	startIndex = g_tid * (numInputLines / g_nthreads);
	endIndex = (g_tid + 1) * (numInputLines / g_nthreads);
	if (g_tid + 1 == g_nthreads && endIndex < numInputLines) {
		endIndex = numInputLines;
	}

#if DEBUG
	cout << "inputLines ->" << endl;
	for (int i = 0; i < inputLines.size(); i++) {
		cout << inputLines[i] << endl;
	}
	cout << "numInputLines -> " << numInputLines << endl;
	cout << "startIndex -> " << startIndex << endl;
	cout << "endIndex -> " << endIndex << endl;
#endif

	map_fp(NULL);
}

void psu_mr_reduce(void *(*reduce_fp)(void *), void *indata, void *outdata) {
#if DEBUG
	cout << "in psu_mr_reduce" << endl;
#endif

	string outFile((const char *) outdata);
	outputFile = outFile;
	fstream oFile;
	oFile.open(outputFile, fstream::out | fstream::trunc);
	oFile.close();

	psu_mutex_lock(0);
	ifstream tempFile(intermediateFile);
	string word;
	string count;
	while (getline(tempFile, word, ':') >> count) {
		if (gWordCount.size() != 0) {
			word.erase(0, 1);
		}
		word.erase(word.size() - 1);
		gWordCount.push_back(tuple<string, int>(word, stoi(count)));
	}

#if DEBUG
	cout << "gWordCount ->" << endl;
	for (int i = 0; i < gWordCount.size(); i++) {
		cout << get<0>(gWordCount[i]) << " : " << get<1>(gWordCount[i]) << "\t";
	}
	cout << endl;
#endif

	psu_mutex_unlock(0);

	numInputWords = gWordCount.size();
	startIndex = g_tid * (numInputWords / g_nthreads);
	endIndex = (g_tid + 1) * (numInputWords / g_nthreads);
	if (g_tid + 1 == g_nthreads && endIndex < numInputWords) {
		endIndex = numInputWords;
	}

#if DEBUG
	cout << "numInputWords -> " << numInputWords << endl;
	cout << "startIndex -> " << startIndex << endl;
	cout << "endIndex -> " << endIndex << endl;
#endif

	reduce_fp(NULL);
}

void psu_mr_destroy() {
	psu_dsm_free();
	psu_stop_lock();
}

void *mapper_wc(void *param) {
#if DEBUG
	cout << "In mapper_wc" << endl;
#endif

	for (int i = startIndex; i < endIndex; i++) {
		vector<string> words;
		if (inputLines[i] != "\n") {
			istringstream iss(inputLines[i]);
			copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(words));
			for (int j = 0; j < words.size(); j++) {
				if (!wordCount.count(words[j])) {
					wordCount[words[j]] = 1;
				}
				else {
					wordCount[words[j]] += 1;
				}
			}
		}
	}

#if DEBUG
	cout << "wordCount ->" << endl;
	map<string, int>::iterator iter;
	for (iter = wordCount.begin(); iter != wordCount.end(); iter++) {
		cout << iter->first << " : " << iter->second << "\t";
	}
	cout << endl;
#endif

	psu_mutex_lock(0);
	fstream tempFile;
	tempFile.open(intermediateFile, fstream::out | fstream::app);
	map<string, int>::iterator it;
	for (it = wordCount.begin(); it != wordCount.end(); it++) {
		string word = it->first;
		if (word.find(':') != string::npos) {
			word.erase(remove(word.begin(), word.end(), ':'), word.end());
		}
		tempFile << word << " : " << it->second << endl;
	}
	tempFile.close();
	psu_mutex_unlock(0);

	barrier(true);

	return NULL;
}

void *reducer_wc(void *param) {
#if DEBUG
	cout << "In reducer_wc" << endl;
#endif
	wordCount.clear();

	psu_mutex_lock(0);
	map<string, int> localWordCount;
	ifstream outFile(outputFile);
	string word;
	string count;
	while (getline(outFile, word, ':') >> count) {
		if (localWordCount.size() != 0) {
			word.erase(0, 1);
		}
		word.erase(word.size() - 1);
		localWordCount[word] = stoi(count);
	}

#if DEBUG
	cout << "localWordCount ->" << endl;
	map<string, int>::iterator iter;
	for (iter = localWordCount.begin(); iter != localWordCount.end(); iter++) {
		cout << iter->first << " : " << iter->second << "\t";
	}
	cout << endl;
#endif

	for (int i = startIndex; i < endIndex; i++) {
		if (!localWordCount.count(get<0>(gWordCount[i]))) {
			if (!wordCount.count(get<0>(gWordCount[i]))) {
				wordCount[get<0>(gWordCount[i])] = get<1>(gWordCount[i]);
				for (int j = 0; j < startIndex; j++) {
					if (get<0>(gWordCount[i]) == get<0>(gWordCount[j])) {
						wordCount[get<0>(gWordCount[i])] += get<1>(gWordCount[j]);
					}
				}
				for (int j = endIndex; j < gWordCount.size(); j++) {
					if (get<0>(gWordCount[i]) == get<0>(gWordCount[j])) {
						wordCount[get<0>(gWordCount[i])] += get<1>(gWordCount[j]);
					}
				}
			}
			else {
				wordCount[get<0>(gWordCount[i])] += get<1>(gWordCount[i]);
			}

		}
	}

#if DEBUG
	cout << "wordCount ->" << endl;
	map<string, int>::iterator itertr;
	for (itertr = wordCount.begin(); itertr != wordCount.end(); itertr++) {
		cout << itertr->first << " : " << itertr->second << "\t";
	}
	cout << endl;
#endif

	fstream oFile;
	oFile.open(outputFile, fstream::out | fstream::app);
	map<string, int>::iterator it;
	for (it = wordCount.begin(); it != wordCount.end(); it++) {
		oFile << it->first << " : " << it->second << endl;
	}
	oFile.close();
	psu_mutex_unlock(0);

	barrier(false);

	/*
	if (!g_tid) {
		oFile.open(outputFile, fstream::in);
		string line;
		while(getline(oFile, line)) {
			cout << line << endl;
		}
		oFile.close();
	}
	*/

	return NULL;
}
