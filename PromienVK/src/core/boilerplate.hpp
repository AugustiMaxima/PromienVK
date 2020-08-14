#ifndef BOILERPLATE_H
#define BOILERPLATE_H

#include "pipeline.hpp"
#include "renderPass.hpp"

namespace core {
	namespace pipeline {
		//sensible defaults
		vk::PipelineInputAssemblyStateCreateInfo configureInputAssembly(vk::PrimitiveTopology topology, bool primitiveRestart);
		ViewportStateEnclosure configureViewport(vk::Extent2D extent);
		vk::PipelineRasterizationStateCreateInfo configureRasterizer();
		vk::PipelineMultisampleStateCreateInfo configureMultisampling();
		vk::PipelineDepthStencilStateCreateInfo configureDepth();
		vk::PipelineColorBlendAttachmentState configureColorBlending();
		ColorBlendingEnclosure configureColorBlendingEnclosure();
		DynamicStateEnclosure configureDynamicState();
		UniformEnclosure configurePipelineLayout();
		GraphicsPipelineEnclosure configureGraphicsPipeline(vk::PrimitiveTopology topology, bool primitiveRestart, vk::Extent2D extent);
	}
}




#endif