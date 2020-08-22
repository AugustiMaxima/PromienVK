#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>

namespace util {
	//In their inifinite wisdom, semaphore was never implemented in C++ standard libraries
	class Semaphore {
		std::atomic<int> count;
		std::mutex lock;
		std::condition_variable cv;

	public:
		Semaphore(int value = 0);
		void P();
		void V();
	};


}



#endif