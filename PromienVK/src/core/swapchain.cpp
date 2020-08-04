#include "swapchain.hpp"

namespace core {
	namespace spc {
		using namespace std;
		using namespace vk;
		SurfaceFormatKHR selectSurfaceFormat(PhysicalDevice device, SurfaceKHR surface, SurfaceFormatKHR req) {
			vector<SurfaceFormatKHR>& sFormats = device.getSurfaceFormatsKHR(surface);
			for (const auto& fm : sFormats) {
				if (fm == req)
					return fm;
			}
			return sFormats[0];
		}
		PresentModeKHR selectPresentMode(PhysicalDevice device, SurfaceKHR surface, PresentModeKHR req) {
			vector<PresentModeKHR>& presentModes = device.getSurfacePresentModesKHR(surface);
			for (const auto& presentMode : presentModes) {
				if (presentMode == req) {
					return presentMode;
				}
			}
			return presentModes[0];
		}
		Extent2D chooseSwapExtent(PhysicalDevice device, SurfaceKHR surface, Extent2D req) {
			SurfaceCapabilitiesKHR scap = device.getSurfaceCapabilitiesKHR(surface);
			if (scap.currentExtent.width != UINT32_MAX) {
				return scap.currentExtent;
			}
			else {
				req.width = req.width > scap.maxImageExtent.width ? scap.maxImageExtent.width : req.width;
				req.width = req.width < scap.minImageExtent.width ? scap.minImageExtent.width : req.width;
				req.height = req.height > scap.maxImageExtent.height ? scap.maxImageExtent.height : req.height;
				req.height = req.height < scap.minImageExtent.height ? scap.minImageExtent.height : req.height;
				return req;
			}
		}
	}
}