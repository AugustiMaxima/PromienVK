#include "InfraVK.hpp"

namespace infr {

	InfraVK::InfraVK() {}

	void InfraVK::run() {
		setup();
		render();
		cleanup();
	}

	InfraVK::~InfraVK() {

	}



}