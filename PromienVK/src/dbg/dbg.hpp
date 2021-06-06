#ifndef DBG
#define DBG

namespace dbg{
#if defined(_DEBUG)
	const bool active = true;
#include <iostream>
using namespace std;
#else
	const bool active = false;
#endif

}




#endif