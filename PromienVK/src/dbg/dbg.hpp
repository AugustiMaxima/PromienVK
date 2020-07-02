#ifndef DBG
#define DBG

namespace dbg{
#if defined(_DEBUG)
	const bool active = true;
#else
	const bool active = false;
#endif

}




#endif