#include "InfraVK.hpp"

namespace infr {

	InfraVK::InfraVK() {}

	void InfraVK::start() {
		createInstance();
		createSurface();
		allocatePhysicalDevices();
		createLogicalDevices();
		createQueues();
		configureSwapChain();
		configureImageView();
		configureGraphicsPipeline();
		render();
	}

	InfraVK::~InfraVK() {

	}



}