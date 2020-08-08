#include "pipeline.hpp"

namespace core {
	namespace pipeline {
		using namespace vk;

		PipelineVertexInputStateCreateInfo VertexInputEnclosure::assemble() {
			verticeInput = PipelineVertexInputStateCreateInfo()
				.setPVertexAttributeDescriptions(attribute.data())
				.setVertexAttributeDescriptionCount(attribute.size())
				.setPVertexBindingDescriptions(binding.data())
				.setVertexBindingDescriptionCount(binding.size());
			return verticeInput;
		}

		PipelineViewportStateCreateInfo ViewportStateEnclosure::assemble() {
			viewportState = PipelineViewportStateCreateInfo()
				.setViewportCount(viewports.size())
				.setPViewports(viewports.data())
				.setScissorCount(scissors.size())
				.setPScissors(scissors.data());
			return viewportState;
		}

		PipelineInputAssemblyStateCreateInfo configureInputAssembly(PrimitiveTopology topology, bool primtiveRestart) {
			return PipelineInputAssemblyStateCreateInfo()
				.setTopology(topology)
				.setPrimitiveRestartEnable(primtiveRestart);
		}

		PipelineViewportStateCreateInfo ViewportStateEnclosure::assemble() {
			viewportState = PipelineViewportStateCreateInfo()
				.setScissorCount(scissors.size())
				.setPScissors(scissors.data())
				.setPViewports(viewports.data())
				.setViewportCount(viewports.size());
			return viewportState;
		}

		PipelineLayoutCreateInfo UniformEnclosure::assemble() {
			layout = PipelineLayoutCreateInfo()
				.setPPushConstantRanges(constants.data())
				.setPushConstantRangeCount(constants.size())
				.setPSetLayouts(descriptors.data())
				.setSetLayoutCount(descriptors.size());
			return layout;
		}

		vk::PipelineLayout UniformEnclosure::construct(Device device) {
			assemble();
			return device.createPipelineLayout(layout);
		}

		PipelineDynamicStateCreateInfo DynamicStateEnclosure::assemble() {
			dynamicState = PipelineDynamicStateCreateInfo()
				.setDynamicStateCount(states.size())
				.setPDynamicStates(states.data());
			return dynamicState;
		}

		PipelineColorBlendStateCreateInfo ColorBlendingEnclosure::assemble() {
			colorBlendingInfo = colorBlendingInfo
				.setAttachmentCount(attachments.size())
				.setPAttachments(attachments.data());
			return colorBlendingInfo;
		}

	}
}