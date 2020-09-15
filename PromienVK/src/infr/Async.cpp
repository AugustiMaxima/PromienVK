#include "Async.hpp"

static void workerThread() {

}

namespace infr {
	namespace asyc {
		Job::~Job(){}
		WaitableJob::~WaitableJob(){}

		Orchestra::Orchestra(int workerCount){
			workers.resize(workerCount);
		}


	}
}