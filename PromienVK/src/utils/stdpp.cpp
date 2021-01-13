#include "stdpp.hpp"

namespace util{
	int stoi(const std::string& pattern, size_t* src, int base) {
		size_t i = src ? *src : 0;
		int num = 0;
		for (; i < pattern.length(); i++) {
			if (pattern[i] <= '9' && pattern[i] >= '0') {
				num *= base;
				num += pattern[i] - '0';
			}
			else {
				break;
			}
		}
		if (src)
			*src = i;
		return num;
	}
}