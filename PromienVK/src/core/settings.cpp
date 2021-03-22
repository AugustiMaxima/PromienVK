#include "settings.hpp"

namespace core {
	DisplaySettings processDisplaySettings(conf::Scope& configs) {
		DisplaySettings set{};
		set.resolution.width = configs["XRes"];
		set.resolution.height = configs["YRes"];
		if (configs["HDR"]) {
			set.format.colorSpace = vk::ColorSpaceKHR::eHdr10St2084EXT;
			set.format.format = vk::Format::eB16G16R16G16422UnormKHR;
		}
		else {
			//standard SRGB
			set.format.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
			set.format.format = vk::Format::eB8G8R8A8Srgb;
		}
		switch (configs["vSync"]) {
		case 0:
			//no vSync
			set.present = vk::PresentModeKHR::eImmediate;
			break;
		case 1:
			//double buffered vsync
			set.present = vk::PresentModeKHR::eFifo;
			break;
		case 2:
			//"relaxed" sync, desirable for variable refresh rate monitors
			set.present = vk::PresentModeKHR::eFifoRelaxed;
			break;
		case 3:
			//triple buffering
			set.present = vk::PresentModeKHR::eMailbox;
			break;
		}

		return set;
	}


	void updateDisplaySettings(conf::Scope& configs, DisplaySettings setting) {
		configs["XRes"] = setting.resolution.width;
		configs["YRes"] = setting.resolution.height;
		if (setting.format.colorSpace == vk::ColorSpaceKHR::eHdr10St2084EXT) {
			configs["HDR"] = 1;
		}
		else {
			configs["HDR"] = 0;
		}
		switch (setting.present) {
		case vk::PresentModeKHR::eImmediate:
			configs["vSync"] = 0;
			break;
		case vk::PresentModeKHR::eFifo:
			configs["vSync"] = 1;
			break;
		case vk::PresentModeKHR::eFifoRelaxed:
			configs["vSync"] = 2;
			break;
		case vk::PresentModeKHR::eMailbox:
			configs["vSync"] = 3;
			break;
		}
	}
}