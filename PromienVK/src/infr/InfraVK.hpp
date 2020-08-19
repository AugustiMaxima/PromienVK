#ifndef INFRA_VK_H
#define INFRA_VK_H
//An abstraction that attempts to formalize resource allocation and management in Vulkan

namespace infr {
	class InfraVK {
	private:
		virtual void createInstance() = 0;
		virtual void createSurface() = 0;
		//devices
		virtual void allocatePhysicalDevices() = 0;
		virtual void createLogicalDevices() = 0;
		//present
		virtual void configureSwapChain() = 0;
		virtual void configureImageView() = 0;
		//pipeline
		virtual void configureRenderPass() = 0;
		virtual void configureGraphicsPipeline() = 0;
		//draw
		virtual void configureFramebuffer() = 0;
		//command
		virtual void configureCommandPool() = 0;
		//iterations
		virtual void render() = 0;
		//cleanup
		virtual void cleanup() = 0;

	public:
		InfraVK();
		virtual void start();
		virtual ~InfraVK();
	};
}
#endif