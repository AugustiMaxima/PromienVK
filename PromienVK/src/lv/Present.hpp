#ifndef PRESENT_HPP
#define PRESENT_HPP

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <vector>

namespace lv {

	struct Surface {
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

}

#endif
