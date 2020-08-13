#ifndef BOILERPLATE_H
#define BOILERPLATE_H

#include "pipeline.hpp"
#include "renderPass.hpp"

namespace core {
	namespace pipeline {
		//sensible defaults
		vk::PipelineInputAssemblyStateCreateInfo configureInputAssembly(vk::PrimitiveTopology topology, bool primitiveRestart);
		ViewportStateEnclosure configureViewport(int width, int height);
		vk::PipelineRasterizationStateCreateInfo configureRasterizer();
		vk::PipelineMultisampleStateCreateInfo configureMultisampling();
		vk::PipelineDepthStencilStateCreateInfo configureDepth();
		vk::PipelineColorBlendAttachmentState configureColorBlending();
		ColorBlendingEnclosure configureColorBlendingEnclosure();
		DynamicStateEnclosure configureDynamicState();
		UniformEnclosure configurePipelineLayout();
	}
}




#endif