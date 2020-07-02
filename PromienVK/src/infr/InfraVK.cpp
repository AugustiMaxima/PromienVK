#include "InfraVK.hpp"

namespace infr {

	InfraVK::InfraVK() {}

	void InfraVK::start() {
		createInstance();
		allocatePhysicalDevices();
		createLogicalDevices();
		createQueues();
		createSurface();
		configureSwapChain();
		configureImageView();
		configureGraphicsPipeline();
		render();
	}

	InfraVK::~InfraVK() {

	}



}