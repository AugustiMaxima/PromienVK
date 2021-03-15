#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vulkan/vulkan.hpp>
#include <vector>

namespace lv {
	struct Device;
	struct CommandBuffer;

	struct CommandPool {
		Device* device;
		int queueIndex;
		vk::CommandPool src;
		std::vector<lv::CommandBuffer> buffers;
	};

	struct CommandBuffer {
		CommandPool* commandPool;
		vk::CommandBuffer src;
	};
}

#endif