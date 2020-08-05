#ifndef PROMIEN_SHADER_HPP
#define PROMIEN_SHADER_HPP

#include <vulkan/vulkan.hpp>

namespace core {
	namespace shader {
		vk::ShaderModule createShaderModule(vk::Device device, const std::string& path);
		vk::PipelineShaderStageCreateInfo createShaderStage(vk::ShaderModule module, vk::ShaderStageFlagBits flag, const std::string& entry = "main");
	}
}


#endif