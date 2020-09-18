#include "Async.hpp"

void workerThread(infr::asyc::Orchestra& orchestra) {
	try {
		while (true) {
			infr::asyc::Job& j = orchestra.requestJob();
			j.start();
		}
	}
	catch (...) {
		//thread termination
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

		void WaitableJob::start(){
			Job::start();
			syc.V();
		}

		void WaitableJob::wait() {
			syc.P();
		}

		WaitableJob::~WaitableJob(){}

		Orchestra::Orchestra(int workerCount):terminal(false){
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
			if (terminal)
				throw std::exception("Terminating Execution");
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
			terminal = true;
			for (int i = 0; i < workers.size(); i++)
				tracker.V();
			for (int i = 0; i < workers.size(); i++) {
				workers[i].join();
			}
		}

	}
}