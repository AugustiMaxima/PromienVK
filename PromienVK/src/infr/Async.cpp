#include "Async.hpp"

void workerThread(infr::asyc::Orchestra& orchestra) {
	while (true) {
		infr::asyc::Job& j = orchestra.requestJob();
		j.start();
	}
}

namespace infr {
	namespace asyc {
		Job::Job() :complete(false){}

		void Job::start(){
			work();
			complete = true;
		}

		bool Job::isComplete() {
			return complete;
		}

		Job::~Job(){}

		WaitableJob::WaitableJob():Job(){}

		WaitableJob::~WaitableJob(){}

		Orchestra::Orchestra(int workerCount){
			workers.resize(workerCount);
		}

		void Orchestra::submitJob(float priority, Job& job) {
			lock.lock(); //Queues have to be externally synchronized
			jobs.insert(priority, &job);
			lock.unlock();
			tracker.V();
		}

		Job& Orchestra::requestJob() {
			tracker.P();
			lock.lock();
			Job* job = jobs.removeMin();
			lock.unlock();
			return *job;
		}

		void Orchestra::orchestrate() {
			for (int i = 0; i < workers.size(); i++) {
				workers[i] = std::thread(workerThread, std::ref(*this));
			}
		}

		Orchestra::~Orchestra() {

		}

	}
}