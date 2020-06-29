#include "InfraVK.hpp"

namespace infr {

	InfraVK::InfraVK() {}

	void InfraVK::start() {
		createInstance();
		configureLayers();
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