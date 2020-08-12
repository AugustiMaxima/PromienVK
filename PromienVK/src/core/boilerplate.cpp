#include "boilerplate.hpp"

namespace core {
	namespace pipeline {
		using namespace vk;
		PipelineInputAssemblyStateCreateInfo configureInputAssembly(PrimitiveTopology topology, bool primtiveRestart) {
			return PipelineInputAssemblyStateCreateInfo()
				.setTopology(topology)
				.setPrimitiveRestartEnable(primtiveRestart);
		}

		ViewportStateEnclosure configureViewport(int width, int height) {
			ViewportStateEnclosure vp;
			vp.viewports.push_back(Viewport()
				.setX(0)
				.setY(0)
				.setHeight(height)
				.setWidth(width)
				.setMinDepth(0)
				.setMaxDepth(1));
			vp.scissors.push_back(vk::Rect2D()
				.setOffset({ 0, 0 })
				.setExtent(vk::Extent2D{ width, height }));
			return vp;
		}

		//only intended for final pass
		//backface culling and depth test should be disabled for voxelization
		PipelineRasterizationStateCreateInfo configureRasterizer() {
			return PipelineRasterizationStateCreateInfo()
				.setDepthClampEnable(false)
				.setRasterizerDiscardEnable(false)
				.setPolygonMode(PolygonMode::eFill)
				.setLineWidth(1)
				.setCullMode(CullModeFlagBits::eNone)
				.setDepthBiasEnable(false)
				.setDepthBiasConstantFactor(0)
				.setDepthBiasClamp(0)
				.setDepthBiasSlopeFactor(0);
		}

		PipelineMultisampleStateCreateInfo configureMultisampling() {
			return PipelineMultisampleStateCreateInfo()
				.setSampleShadingEnable(false)
				.setRasterizationSamples(vk::SampleCountFlagBits::e1)
				.setMinSampleShading(1)
				.setPSampleMask(nullptr)
				.setAlphaToCoverageEnable(false)
				.setAlphaToOneEnable(false);
		}

		PipelineDepthStencilStateCreateInfo configureDepth() {
			return PipelineDepthStencilStateCreateInfo()
				.setDepthTestEnable(true);
		}

		PipelineColorBlendAttachmentState configureColorBlending() {
			return PipelineColorBlendAttachmentState()
				.setColorWriteMask(ColorComponentFlagBits::eR | ColorComponentFlagBits::eG | ColorComponentFlagBits::eB | ColorComponentFlagBits::eA)
				.setBlendEnable(false);
		}

		ColorBlendingEnclosure configureColorBlendingEnclosure() {
			ColorBlendingEnclosure cb;
			cb.attachments.push_back(configureColorBlending());
			cb.colorBlendingInfo
				.setLogicOpEnable(false);
			return cb;
		}

		DynamicStateEnclosure configureDynamicState() {
			DynamicStateEnclosure ds;
			ds.states.push_back(DynamicState::eViewport);
			return ds;
		}

		UniformEnclosure configurePipelineLayout() {
			UniformEnclosure ue;
			//To be customized for each app
			return ue;
		}

	}
}