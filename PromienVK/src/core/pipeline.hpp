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
		
		struct ViewportStateEnclosure{
			vk::PipelineViewportStateCreateInfo viewportState{};
			std::vector<vk::Viewport> viewports;
			std::vector<vk::Rect2D> scissors;
			void assemble();
		};

		struct VertexInputEnclosure {
			std::vector<vk::PipelineVertexInputStateCreateInfo> verticeInput;
			std::vector<vk::PipelineInputAssemblyStateCreateInfo> inputAssembly;
		};

		struct UniformEnclosure {
			vk::PipelineLayoutCreateInfo layout;
			std::vector<vk::DescriptorSetLayout> descriptors;
			std::vector<vk::PushConstantRange> constants;
			void assemble();
			vk::PipelineLayout construct();
		};

		struct DynamicStateEnclosure {
			vk::PipelineDynamicStateCreateInfo dynamicState;
			std::vector<vk::DynamicState> states;
			void assemble();
		};

		struct ColorBlendingEnclosure {
			vk::PipelineColorBlendStateCreateInfo colorBlendingInfo;
			std::vector <vk::PipelineColorBlendAttachmentState> attachments;
			void assemble();
		};

		struct GraphicsPipelineEnclosure {
			vk::GraphicsPipelineCreateInfo info;
			ViewportStateEnclosure viewports;
			VertexInputEnclosure vertice;
			vk::PipelineRasterizationStateCreateInfo rasterizer;
			//literally useless in 2020
			vk::PipelineMultisampleStateCreateInfo multisampling;
			vk::PipelineDepthStencilStateCreateInfo stencil;
			DynamicStateEnclosure dynamicState;
			ColorBlendingEnclosure colorBlending;
			UniformEnclosure uniform;
			void assemble();
			vk::Pipeline construct();
		};
	}
}

#endif