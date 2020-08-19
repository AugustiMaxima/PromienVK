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
		configureRenderPass();
		configureGraphicsPipeline();
		configureFramebuffer();
		configureCommandPool();
		render();
		cleanup();
	}

	InfraVK::~InfraVK() {

	}



}