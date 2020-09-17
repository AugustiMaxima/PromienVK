#ifndef ASYNC_H
#define ASYNC_H

#include "../utils/semaphore.hpp"
#include "../utils/heap.hpp"
#include <thread>
#include <vector>
#include <mutex>

namespace infr {
	namespace asyc {

		class Job {
			volatile bool complete;
		public:
			Job();
			virtual void work() = 0;
			void start();
			bool isComplete();
			virtual ~Job();
		};

		class WaitableJob : public Job {
		public:
			WaitableJob();
			virtual void wait() = 0;
			virtual ~WaitableJob();
		};

		class Orchestra {
			std::mutex lock;
			util::Semaphore tracker;
			std::vector<std::thread> workers;
			util::minHeap<float, Job*> jobs;
		public:
			Orchestra(int workerCount = 4);
			void submitJob(float priority, Job& job);
			Job& requestJob();
			void orchestrate();
			~Orchestra();
		};


	}
}



#endif