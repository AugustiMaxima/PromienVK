#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vulkan/vulkan.hpp>
#include <vector>

namespace lv {
	struct Device;

	struct CommandPool {
		Device* src;
		vk::CommandPool pool;
		std::vector<vk::CommandBuffer> buffers;
	};
}


#endif