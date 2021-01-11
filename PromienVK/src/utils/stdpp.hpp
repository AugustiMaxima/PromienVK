#ifndef STD_PP

#include <string>

//std functions like from the c++ library, but more competently designed

namespace util {
	int stoi(const std::string& pattern, size_t* src = nullptr, int base = 10);
}


#endif
