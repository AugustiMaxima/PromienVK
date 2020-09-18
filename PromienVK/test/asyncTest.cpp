#include <iostream>
#include <vector>
#include "masterTest.hpp"
#include "../src/infr/Async.hpp"

class atomicTest : public infr::asyc::WaitableJob {
	util::Semaphore syc;
public:
	int base;
	atomicTest():WaitableJob(){}
	atomicTest(int base):WaitableJob(), base(base){}
	atomicTest& operator=(const atomicTest& at) {
		base = at.base;
		return *this;
	}
	void work() {
		using namespace std;
		base++;
		syc.V();
	}
	void wait() {
		syc.P();
	}
};


int asyncTest() {
	using namespace std;
	using namespace infr::asyc;
	Orchestra flagship{ 8 };
	vector<atomicTest> atoms(100);
	for (int i = 0; i < atoms.size(); i++) {
		atoms[i] = atomicTest(i);
		flagship.submitJob(1, atoms[i]);
	}
	flagship.orchestrate();
	for (int i = 0; i < atoms.size(); i++) {
		atoms[i].wait();
		if (atoms[i].base != i + 1) {
			throw "Synchronization failed";
		}
	}

	cout << "Async Test passed" << endl;
	return 0;
}