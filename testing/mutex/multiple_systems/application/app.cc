#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>

#include "mutex.h"

using namespace std;

int main(int argc, char* argv[]) {
	bool status = initMutex();
	if (!status) {
		return 0;
	}
	string hostName = GetHostName();
	enterMutex();
	// Critical section begin
	cout << "CS begin" << endl;
	cout << time(0) << endl;
	cout << hostName << endl;
	this_thread::sleep_for(chrono::milliseconds(5000));
	cout << time(0) << endl;
	cout << "CS end" << endl;
	// Critical section end
	exitMutex();
	destroyMutex();
	return 0;
}
