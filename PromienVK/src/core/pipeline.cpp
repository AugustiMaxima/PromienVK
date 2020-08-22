#include "shader.hpp"
#include "pipeline.hpp"

namespace core {
	namespace pipeline {
		using namespace vk;

		std::vector<vk::PipelineShaderStageCreateInfo>& ShaderAssemblerEnclosure::assemble(vk::Device device) {
			if (info.size())
				return info;
			if (!shaders.size()) {
				for (const auto& src : srcs) {
					shaders.push_back(shader::createShaderModule(device, src));
				}
			}
			//assuming srcs, entryname and stages are equal
			for (int i = 0; i < shaders.size(); i++) {
				info.push_back(PipelineShaderStageCreateInfo()
					.setModule(shaders[i])
					.setPName(entryName[i].data())
					.setStage(stages[i]));
			}
			return info;
		}

		PipelineVertexInputStateCreateInfo& VertexInputEnclosure::assemble() {
			verticeInput = verticeInput
				.setPVertexAttributeDescriptions(attribute.data())
				.setVertexAttributeDescriptionCount(attribute.size())
				.setPVertexBindingDescriptions(binding.data())
				.setVertexBindingDescriptionCount(binding.size());
			return verticeInput;
		}

		PipelineViewportStateCreateInfo& ViewportStateEnclosure::assemble() {
			viewportState = viewportState
				.setViewportCount(viewports.size())
				.setPViewports(viewports.data())
				.setScissorCount(scissors.size())
				.setPScissors(scissors.data());
			return viewportState;
		}

		PipelineLayoutCreateInfo& UniformEnclosure::assemble() {
			info = info
				.setPPushConstantRanges(constants.size()? constants.data() : nullptr)
				.setPushConstantRangeCount(constants.size())
				.setPSetLayouts(descriptors.size()? descriptors.data() : nullptr)
				.setSetLayoutCount(descriptors.size());
			return info;
		}

		vk::PipelineLayout& UniformEnclosure::construct(Device device) {
			if (constructed)
				return layout;
			layout = device.createPipelineLayout(assemble());
			constructed = true;
			return layout;
		}

		PipelineDynamicStateCreateInfo& DynamicStateEnclosure::assemble() {
			dynamicState = dynamicState
				.setDynamicStateCount(states.size())
				.setPDynamicStates(states.data());
			return dynamicState;
		}

		PipelineColorBlendStateCreateInfo& ColorBlendingEnclosure::assemble() {
			colorBlendingInfo = colorBlendingInfo
				.setAttachmentCount(attachments.size())
				.setPAttachments(attachments.data());
			return colorBlendingInfo;
		}

		GraphicsPipelineCreateInfo& GraphicsPipelineEnclosure::assemble(Device device, RenderPass renderpass, int subpass, vk::Pipeline base = nullptr, int baseIndex = -1) {
			info = info
				.setStageCount(shaders.assemble(device).size())
				.setPStages(shaders.assemble(device).data())
				.setPVertexInputState(&vertice.assemble())
				.setPInputAssemblyState(&input)
				.setPTessellationState(&tesslation)
				.setPViewportState(&viewports.assemble())
				.setPRasterizationState(&rasterizer)
				.setPMultisampleState(&multisampling)
				.setPDepthStencilState(&stencil)
				.setPDynamicState(&dynamicState.assemble())
				.setPColorBlendState(&colorBlending.assemble())
				.setLayout(uniform.construct(device))
				.setRenderPass(renderpass)
				.setSubpass(subpass)
				.setBasePipelineHandle(base)
				.setBasePipelineIndex(baseIndex);
			return info;
		}

		Pipeline& GraphicsPipelineEnclosure::construct(Device device, RenderPass renderpass, int subpass, Pipeline base, int baseIndex, PipelineCache cache) {
			pipeline = device.createGraphicsPipeline(cache, assemble(device, renderpass, subpass, base, baseIndex));
			return pipeline;
		}
	}
}