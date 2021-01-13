#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.hpp>
#include <vector>

namespace core {
	namespace pipeline {
		//One of the most terrible aspect of vulkan's design lies in its C style roots
		//pointer based ownership means that one cannot decouple configuration of individual parameters from the root level structs
		//this strongly discourage modularity and prevents effective refactor and code reuse

		//So we are going to attempt to fix the cancer that is vanilla vulkan by using structs to build enclosures

		struct ShaderAssemblerEnclosure {
			std::vector<std::string> srcs;
			std::vector<std::string> entryName;
			std::vector<vk::ShaderStageFlagBits> stages;
			std::vector<vk::ShaderModule> shaders;
			std::vector<vk::PipelineShaderStageCreateInfo> info;
			std::vector<vk::PipelineShaderStageCreateInfo>& assemble(vk::Device device);
		};

		struct VertexInputEnclosure {
			vk::PipelineVertexInputStateCreateInfo verticeInput;
			std::vector<vk::VertexInputBindingDescription> binding;
			std::vector<vk::VertexInputAttributeDescription> attribute;
			vk::PipelineVertexInputStateCreateInfo& assemble();
		};

		struct ViewportStateEnclosure{
			vk::PipelineViewportStateCreateInfo viewportState{};
			std::vector<vk::Viewport> viewports;
			std::vector<vk::Rect2D> scissors;
			vk::PipelineViewportStateCreateInfo& assemble();
		};

		struct ColorBlendingEnclosure {
			vk::PipelineColorBlendStateCreateInfo colorBlendingInfo;
			std::vector <vk::PipelineColorBlendAttachmentState> attachments;
			vk::PipelineColorBlendStateCreateInfo& assemble();
		};

		struct DynamicStateEnclosure {
			vk::PipelineDynamicStateCreateInfo dynamicState;
			std::vector<vk::DynamicState> states;
			vk::PipelineDynamicStateCreateInfo& assemble();
		};

		struct UniformEnclosure {
			bool constructed;
			vk::PipelineLayoutCreateInfo info;
			std::vector<vk::DescriptorSetLayout> descriptors;
			std::vector<vk::PushConstantRange> constants;
			vk::PipelineLayout layout;
			vk::PipelineLayoutCreateInfo& assemble();
			vk::PipelineLayout& construct(vk::Device device);
		};

		struct GraphicsPipelineEnclosure {
			vk::GraphicsPipelineCreateInfo info;
			ShaderAssemblerEnclosure shaders;
			VertexInputEnclosure vertice;
			vk::PipelineInputAssemblyStateCreateInfo input;
			vk::PipelineTessellationStateCreateInfo tesslation;
			ViewportStateEnclosure viewports;
			vk::PipelineRasterizationStateCreateInfo rasterizer;
			//literally useless in 2020
			vk::PipelineMultisampleStateCreateInfo multisampling;
			vk::PipelineDepthStencilStateCreateInfo stencil;
			DynamicStateEnclosure dynamicState;
			ColorBlendingEnclosure colorBlending;
			UniformEnclosure uniform;
			vk::Pipeline pipeline;
			vk::GraphicsPipelineCreateInfo& assemble(vk::Device device, vk::RenderPass renderpass, int subpass, vk::Pipeline base, int baseIndex);
			vk::Pipeline& construct(vk::Device device, vk::RenderPass renderpass, int subpass, vk::Pipeline base, int baseIndex, vk::PipelineCache cache);
		};

	}
}

#endif