#include <iostream>

#include "psu_dsm.h"

using namespace std;

int main(int argc, char* argv[]) {
	bool status = initDir();
	if (!status) {
		return 0;
	}
	cout << "Directory initialized" << endl;
	stopDir();
	return 0;
}
