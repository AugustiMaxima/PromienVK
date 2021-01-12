#include <iostream>
#include "../src/infr/hvm.hpp"
#include "../src/infr/lvm.hpp"
#include "masterTest.hpp"

using namespace infr;
using namespace std;

static int overAllocTest() {
	lvm::LinearVM vm{ 2048 };
	vm.malloc(1024);
	vm.malloc(512);
	vm.malloc(256);
	try {
		vm.malloc(1024);
	}
	catch (exception& exp) {
		cout << "Allocation budget exceeded! - Caught" << endl;
		return 0;
	}

	throw exception("Alloc budget exceeded and was not detected");
}

static int rightAllocTest() {
	hvm::HierarchicalVM vm{ 2048, 1 };
	uint64_t ptr[100];
	ptr[0] = vm.malloc(1024);
	ptr[1] = vm.malloc(512);
	ptr[2] = vm.malloc(256);
	ptr[3] = vm.malloc(128);
	ptr[4] = vm.malloc(64);
	ptr[5] = vm.malloc(64);
	vm.free(ptr[4]);
	vm.free(ptr[3]);
	vm.free(ptr[5]);
	ptr[6] = vm.malloc(256);
	vm.free(ptr[2]);
	vm.free(ptr[1]);
	vm.free(ptr[6]);
	ptr[7] = vm.malloc(1024);
	vm.free(ptr[1]);
	vm.free(ptr[0]);
	ptr[8] = vm.malloc(2048);
	cout << "Expected: 2048 - 256 = " << ptr[6] << endl;;
	cout << "Expected: 2048 - 2048 = " << ptr[8] << endl;;
	return 0;
}

static int alignStressTest(const int epoch) {
	uint64_t limit = 9663676416;
	uint64_t bound = limit * 84 / 100;
	lvm::LinearVM vm{ limit };

	uint64_t ramtally = 0;
	std::vector<uint64_t> batches;

	for (uint64_t i = 1; i < limit / 4; i = i * 2) {
		batches.push_back(i);
	}

	uint64_t e = 0;

	std::vector<uint64_t> alloced;
	std::vector<uint64_t> sizes;
	uint64_t dex = 0;

	while (e++ < epoch) {
		uint64_t action = e % 3;
		//cout << e << endl;
		if (action < 2) {
			uint64_t alx = e * 67 % batches.size();
			uint64_t dev = e * 17 % (alx + 1);
			uint64_t plier = action % 2 ? -1 : 1;
			uint64_t fsize = batches[alx] + plier * batches[dev];
			if (fsize && ramtally + fsize <= bound) {
				//cout << "Alloccing " << fsize << " bytes" << endl;
				alloced.push_back(vm.malloc(fsize));
				//cout << "Allocated at " << alloced[alloced.size() - 1] << endl;
				sizes.push_back(fsize);
				ramtally += fsize;
			}
		}
		else {
			if (alloced.size() > dex) {
				uint64_t kek = (e * 13 % (alloced.size() - dex)) + dex;
				uint64_t temp = alloced[dex];
				uint64_t temp2 = sizes[dex];
				alloced[dex] = alloced[kek];
				sizes[dex] = sizes[kek];
				alloced[kek] = temp;
				sizes[kek] = temp2;
				//cout << "Freeing " << sizes[dex] << " bytes at " << alloced[dex] << endl;
				vm.free(alloced[dex]);
				//cout << "Freed at " << alloced[dex] << endl;
				ramtally -= sizes[dex++];
			}
		}
	}

	//cout << "Pretty stable performance!" << endl;

	return 0;
}



int allocTest(bool stress) {
	overAllocTest();
	rightAllocTest();

	alignStressTest(stress? 10000000 : 10000);
	return 0;
}