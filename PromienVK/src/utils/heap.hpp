#ifndef HEAP_H
#define HEAP_H

#include <vector>

namespace util {

	template<typename K, typename V>
	class minHeap {
		std::vector<std::pair<K, V>> entries;
	public:
		void heapFloat() {
			int p = entries.size() - 1;
			int np = p / 2;
			while (entries[p].first < entries[np].first) {
				auto temp = entries[p];
				entries[p] = entries[np];
				entries[np] = temp;
				p = np;
				np = p / 2;
			}
		}
		void heapSink() {
			int p = 0;
			while (2*p + 1 < entries.size()) {
				int np = entries[2 * p + 1] > (2 * p + 2 < entires.size() ? entries[2 * p + 2] : 0) ? 2 * p + 1 : 2 * p + 2;
				if (entries[np] < entries[p]) {
					auto temp = entries[p];
					entries[p] = entries[np];
					entries[np] = temp;
					p = np;
				}
				else
					break;
			}
		}

		void insert(K priority, V value) {
			entries.push_back(std::pair(priority, value));
			heapFloat();
		}

		V removeMin() {
			V ret = entries[entries.size() - 1];
			entries.pop_back();
			return ret;
		}
	};



}











#endif