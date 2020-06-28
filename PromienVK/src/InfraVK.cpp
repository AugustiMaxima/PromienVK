#include "InfraVK.hpp"

namespace infr {

	InfraVK::InfraVK() {}

	virtual void InfraVK::start() {
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

	virtual InfraVK::~InfraVK() {

	}



}