#include "masterTest.hpp"
#include "../src/utils/heap.hpp"

int heapTest() {
	using namespace util;
	using namespace std;
	minHeap<int, int> heap;
	std::vector<int> arr = {4,3,6,7,4,2,4,6,78,98,231,2,1,3,2,6,7,5,4,56};

	for (int i = 0; i < arr.size(); i++) {
		heap.insert(arr[i], arr[i]);
	}

	int ret = 0;
	for (int i = 0; i < arr.size(); i++) {
		int n = heap.removeMin();
		if (ret > n) {
			throw "Unstable order";
		}
		ret = n;
	}

	return 0;
}
