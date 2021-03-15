#ifndef PRESENT_HPP
#define PRESENT_HPP

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <vector>

namespace lv {
	struct Instance;

	struct Surface {
		Instance* instance;
		GLFWwindow* window;
		vk::SurfaceKHR surface;
	};

	struct Device;

	struct SwapChain {
		Device* device;
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageViews;
	};

	struct RenderPass;

	struct FrameBuffer {
		SwapChain* swapchain;
		RenderPass* renderPass;
		std::vector<vk::Framebuffer> framebuffers;
	};

	struct FrameSynchronization {
		FrameBuffer* framebuffer;
		int framesInFlight;
		std::vector<vk::Semaphore> imageAcquired;
		std::vector<vk::Semaphore> renderComplete;
		std::vector<vk::Fence> frameFinished;
		std::vector<vk::Fence> frameLease;
	};
}

#endif
