#include <iostream>
#include "../src/infr/hvm.hpp"
#include "masterTest.hpp"

using namespace infr::hvm;
using namespace std;

static int overAllocTest() {
	HierarchicalVM vm{ 2048, 1 };
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
	HierarchicalVM vm{ 2048, 1 };
	int ptr[100];
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

static int allocStressTest() {
	int limit = 4194304;
	int bound = limit*9/10;
	//A word on the fragmentation efficiency
	//100% is impossible for any architecture
	//the theoretical worst case for this allocator should be 50%
	//in practise, i expect 75% to be the average utilization
	//in this particular randomized "real world" benchmark, it seems to achieve 90% utilization
	//that's way better than I can reasonably expect, but is quite exciting
	HierarchicalVM vm{limit, 1, 1};

	int ramtally = 0;
	std::vector<int> batches;

	for (int i = 1; i < bound/4; i=i*2) {
		batches.push_back(i);
	}

	int e = 0;

	std::vector<int> alloced;
	std::vector<int> sizes;
	int dex = 0;

	while (e++ < 1000000) {
		int action = e % 3;
		cout << e << endl;
		if (action < 2) {
			int alx = e * 67 % batches.size();
			if (ramtally + batches[alx] <= bound) {
				cout << "Alloccing " << batches[alx] << " bytes" << endl;
				alloced.push_back(vm.malloc(batches[alx]));
				cout << "Allocated at " << alloced[alloced.size() - 1] << endl;
				sizes.push_back(batches[alx]);
				ramtally += batches[alx];
			}
		}
		else {
			if (alloced.size() > dex) {
				int kek = (e * 13 % (alloced.size() - dex)) + dex;
				int temp = alloced[dex];
				int temp2 = sizes[dex];
				alloced[dex] = alloced[kek];
				sizes[dex] = sizes[kek];
				alloced[kek] = temp;
				sizes[kek] = temp2;
				vm.free(alloced[dex]);
				cout << "Freeing " << sizes[dex] << " bytes" << endl;
				cout << "Freed at " << alloced[dex] << endl;
				ramtally -= sizes[dex++];
			}
		}
		vm.errorChecking();
	}

	cout << "Pretty stable performance!" << endl;

	return 0;
}


static int alignStressTest() {
	int limit = 9663676416;
	int bound = limit * 9 / 10;
	//A word on the fragmentation efficiency
	//100% is impossible for any architecture
	//the theoretical worst case for this allocator should be 50%
	//in practise, i expect 75% to be the average utilization
	//in this particular randomized "real world" benchmark, it seems to achieve 90% utilization
	//that's way better than I can reasonably expect, but is quite exciting
	HierarchicalVM vm{ limit, 16, 16 };

	int ramtally = 0;
	std::vector<int> batches;

	for (int i = 1; i < bound / 4; i = i * 2) {
		batches.push_back(i);
	}

	int e = 0;

	std::vector<int> alloced;
	std::vector<int> sizes;
	int dex = 0;

	while (e++ < 1000000) {
		int action = e % 3;
		cout << e << endl;
		if (action < 2) {
			int alx = e * 67 % batches.size();
			if (ramtally + batches[alx] <= bound) {
				cout << "Alloccing " << batches[alx] << " bytes" << endl;
				alloced.push_back(vm.malloc(batches[alx]));
				cout << "Allocated at " << alloced[alloced.size() - 1] << endl;
				sizes.push_back(batches[alx]);
				ramtally += batches[alx];
			}
		}
		else {
			if (alloced.size() > dex) {
				int kek = (e * 13 % (alloced.size() - dex)) + dex;
				int temp = alloced[dex];
				int temp2 = sizes[dex];
				alloced[dex] = alloced[kek];
				sizes[dex] = sizes[kek];
				alloced[kek] = temp;
				sizes[kek] = temp2;
				vm.free(alloced[dex]);
				cout << "Freeing " << sizes[dex] << " bytes" << endl;
				cout << "Freed at " << alloced[dex] << endl;
				ramtally -= sizes[dex++];
			}
		}
		vm.errorChecking();
	}

	cout << "Pretty stable performance!" << endl;

	return 0;
}



int allocTest(bool stress) {
	overAllocTest();
	rightAllocTest();

	if (!stress)
		return 0;

	allocStressTest();
	alignStressTest();
	return 0;
}