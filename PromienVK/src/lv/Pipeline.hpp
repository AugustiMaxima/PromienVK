#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vulkan/vulkan.hpp>

namespace lv {
	struct Device;
	struct RenderPass;

	struct Pipeline {
		RenderPass* renderPass;
		vk::Pipeline pipeline;
		vk::PipelineLayout layout;
	};

	struct RenderPass {
		Device* src;
		vk::RenderPass renderPass;
		std::vector<Pipeline> pipelines;
	};
}

#endif