#ifndef NAIVE_HPP
#define NAIVE_HPP

namespace lv {
	struct Instance;
	struct Surface;
	struct PhysicalDevice;
	struct Device;
}

namespace lvl {

	void configureInstance(lv::Instance& instance, std::string& appName, uint32_t version);

	void enumeratePhysicalDevices(lv::Instance& instance);

	void initializeSurface(lv::Instance& instance, lv::Surface& surface, int width, int height, std::string& appName);

	lv::PhysicalDevice* collectRenderingDevice(lv::Instance& instance, lv::Surface& surface);

	void initializeSwapchain(lv::SwapChain& swapchain, lv::Device& device, lv::Surface& surf, core::settings::DisplaySettings& display);

	void initializeFrameBuffer(lv::FrameBuffer& frameBuffer, lv::SwapChain& swapchain, lv::RenderPass& renderPass, core::settings::DisplaySettings& display);

	void initializeFrameSynchronization(lv::FrameSynchronization& sync, lv::FrameBuffer& frameBuffer, int framesInFlight = -1);
}

#endif