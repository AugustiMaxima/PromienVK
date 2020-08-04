#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.hpp>
#include <vector>

namespace core {
	namespace spc {
		vk::SurfaceFormatKHR selectSurfaceFormat(vk::PhysicalDevice device, vk::SurfaceKHR surface, vk::SurfaceFormatKHR req);
		vk::PresentModeKHR selectPresentMode(vk::PhysicalDevice device, vk::SurfaceKHR surface, vk::PresentModeKHR req);
		vk::Extent2D chooseSwapExtent(vk::PhysicalDevice device, vk::SurfaceKHR surface, vk::Extent2D req);
	}
}


#endif