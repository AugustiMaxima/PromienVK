#include "../lv/lv.hpp"
#include "settings.hpp"
#include "../infr/type.hpp"
#include "../asset/io/loader.hpp"
#include "../asset/format/obj.hpp"

namespace core {
	class Harness {
		lv::Instance instance;
		lv::PhysicalDevice* pDevice;
		lv::Device* device;
		std::vector<vk::Queue> graphicQueues;
		std::vector<vk::Queue> transferQueues;
		lv::Surface surface;
		lv::SwapChain swapchain;
		lv::RenderPass renderPass;
		lv::FrameBuffer frameBuffer;
		lv::FrameSynchronization synchronization;
		core::DisplaySettings display;

		asset::io::StreamHost host;
		asset::format::Model* lux;

		asset::io::Vueue vram;

		void renderFrame(unsigned f);

	public:
		Harness(const std::string& config);
		void start();
		~Harness();
	};
}