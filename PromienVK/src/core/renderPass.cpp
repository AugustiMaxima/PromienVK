#include "renderPass.hpp"

namespace core {
	namespace pipeline {

		vk::SubpassDescription& SubpassEnclosure::assemble() {
			subpass = subpass
				.setInputAttachmentCount(input.size())
				.setPInputAttachments(input.data())
				.setColorAttachmentCount(color.size())
				.setPColorAttachments(color.data())
				.setPResolveAttachments(resolve.size() ? resolve.data() : nullptr)
				.setPDepthStencilAttachment(&stencil)
				.setPreserveAttachmentCount(preserve.size())
				.setPPreserveAttachments(preserve.data());
			return subpass;
		}

		vk::RenderPassCreateInfo& RenderPassEnclosure::assemble() {
			if (subpasses.size())
				return info;
			for (auto& sbs : subpassInf) {
				subpasses.push_back(sbs.assemble());
			}
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
			renderPass = device.createRenderPass(assemble());
			return renderPass;
		}
	}
}