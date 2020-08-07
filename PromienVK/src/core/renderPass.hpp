#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.hpp>
#include <vector>

namespace core {
	namespace pipeline {
		struct RenderPassEnclosure {
			vk::RenderPassCreateInfo info;
			std::vector<vk::AttachmentDescription> attachments;
			std::vector<vk::SubpassDescription> subpasses;
			std::vector<vk::AttachmentReference> attachRefs;
			void assemble();
			vk::RenderPass construct();
		};
	}
}



#endif