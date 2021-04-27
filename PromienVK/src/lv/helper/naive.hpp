#ifndef NAIVE_HPP
#define NAIVE_HPP

#include "../../core/settings.hpp"

namespace lv {
	struct Instance;
	struct Surface;
	struct PhysicalDevice;
	struct Device;
	struct SwapChain;
	struct FrameBuffer;
	struct FrameSynchronization;
	struct RenderPass;
}

namespace lvl {

	void configureInstance(lv::Instance& instance, std::string& appName, uint32_t version);

	void enumeratePhysicalDevices(lv::Instance& instance);

	void initializeSurface(lv::Instance& instance, lv::Surface& surface, int width, int height, std::string& appName);

	lv::PhysicalDevice* collectRenderingDevice(lv::Instance& instance, lv::Surface& surface);

	void initializeSwapchain(lv::SwapChain& swapchain, lv::Device& device, lv::Surface& surf, core::DisplaySettings& display);

	void initializeFrameBuffer(lv::FrameBuffer& frameBuffer, lv::SwapChain& swapchain, lv::RenderPass& renderPass, core::DisplaySettings& display);

	void initializeFrameSynchronization(lv::FrameSynchronization& sync, lv::FrameBuffer& frameBuffer, int queueIndex, int framesInFlight = -1);

}

#endif