
#include "semaphore.hpp"

namespace util {

	Semaphore::Semaphore(int value) :count(value) {

	}

	void Semaphore::P() {
		std::unique_lock<std::mutex> lk{ lock };
		while (count == 0);
			cv.wait(lk);
		count--;
		lk.unlock();
	}


	void Semaphore::V() {
		std::unique_lock<std::mutex> lk{ lock };
		count++;
		lk.unlock();
		cv.notify_one();
	}



}