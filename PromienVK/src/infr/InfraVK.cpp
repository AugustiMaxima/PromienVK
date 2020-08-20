#include "InfraVK.hpp"

namespace infr {

	InfraVK::InfraVK() {}

	void InfraVK::start() {
		setup();
		render();
		cleanup();
	}

	InfraVK::~InfraVK() {

	}



}