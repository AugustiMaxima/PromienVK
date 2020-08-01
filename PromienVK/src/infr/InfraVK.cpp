#include "InfraVK.hpp"

namespace infr {

	InfraVK::InfraVK() {}

	void InfraVK::start() {
		createInstance();
		createSurface();
		allocatePhysicalDevices();
		createLogicalDevices();
		configureSwapChain();
		configureImageView();
		configureGraphicsPipeline();
		render();
		cleanup();
	}

	InfraVK::~InfraVK() {

	}



}