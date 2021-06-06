#ifndef SETTINGS_H
#define SETTINGS_H

#include "../infr/Conf.hpp"
#include <vulkan/vulkan.hpp>

namespace core{
	struct DisplaySettings {
		vk::SurfaceFormatKHR format;
		vk::PresentModeKHR present;
		vk::Extent2D resolution;
	};

	DisplaySettings processDisplaySettings(conf::Scope& configs);
	void updateDisplaySettings(conf::Scope& configs, DisplaySettings setting);
}

#endif