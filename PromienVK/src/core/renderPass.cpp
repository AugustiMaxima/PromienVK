#include "renderPass.hpp"

namespace core {
	namespace pipeline {
		vk::RenderPassCreateInfo& RenderPassEnclosure::assemble() {
			info = info
				.setAttachmentCount(attachments.size())
				.setPAttachments(attachments.data())
				.setSubpassCount(subpasses.size())
				.setPSubpasses(subpasses.data())
				.setDependencyCount(dependencies.size())
				.setPDependencies(dependencies.data());
			return info;
		}

		vk::RenderPass& RenderPassEnclosure::construct(vk::Device device){
			renderPass = device.createRenderPass(info);
			return renderPass;
		}
	}
}