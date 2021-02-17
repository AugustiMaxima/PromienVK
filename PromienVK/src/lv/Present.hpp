#ifndef PRESENT_HPP
#define PRESENT_HPP

#include <vulkan/vulkan.hpp>
#include <vector>

namespace lv {

	struct Device;

	struct SwapChain {
		Device* device;
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageViews;
	};

}

#endif
