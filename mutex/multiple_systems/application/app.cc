#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>

#include "psu_mutex.h"

using namespace std;

int main(int argc, char* argv[]) {
	bool status = psu_start_lock();
	if (!status) {
		return 0;
	}
	psu_init_lock(1);
	string hostName = GetHostName();
	psu_mutex_lock(1);
	// Critical section begin
	cout << "CS begin" << endl;
	cout << time(0) << endl;
	cout << hostName << endl;
	this_thread::sleep_for(chrono::milliseconds(5000));
	cout << time(0) << endl;
	cout << "CS end" << endl;
	// Critical section end
	psu_mutex_unlock(1);
	// psu_destroy_lock(1);
	psu_stop_lock();
	return 0;
}
