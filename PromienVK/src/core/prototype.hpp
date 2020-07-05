#ifndef PROTOTYPE
#define PROTOTYPE

#include <GLFW/glfw3.h>
#include "../infr/Base.hpp"

namespace core {
	class Prototype : public infr::Base {
		void initWindow();
	protected:
		//We have integrated GLFW to provide cross plat compatibility
		GLFWwindow* window;
		virtual void createInstance();
		virtual void createSurface();
		virtual void allocatePhysicalDevices();
		virtual void createLogicalDevices();
		virtual void createQueues();
		virtual void configureSwapChain();
		virtual void configureImageView();
		virtual void configureGraphicsPipeline();
		virtual void render();
		virtual void cleanup();

	public:
		Prototype(std::string config);
		virtual ~Prototype();
	};
}

#endif