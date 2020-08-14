#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.hpp>
#include <vector>

namespace core {
	namespace pipeline {

		struct SubpassEnclosure {
			std::vector<vk::AttachmentReference> input;
			std::vector<vk::AttachmentReference> color;
			std::vector<vk::AttachmentReference> resolve; // multisampling
			vk::AttachmentReference stencil;
			std::vector<uint32_t> preserve;
			vk::SubpassDescription subpass;
			vk::SubpassDescription& assemble();
		};

		struct RenderPassEnclosure {
			vk::RenderPassCreateInfo info;
			std::vector<vk::AttachmentDescription> attachments;
			std::vector<SubpassEnclosure> subpassInf;
			std::vector<vk::SubpassDescription> subpasses;
			std::vector<vk::SubpassDependency> dependencies;
			vk::RenderPass renderPass;
			vk::RenderPassCreateInfo& assemble();
			vk::RenderPass& construct(vk::Device device);
		};
	}
}



#endif