#include "../lv/lv.hpp"
#include "../infr/type.hpp"
#include "../asset/io/loader.hpp"
#include "../asset/format/obj.hpp"

namespace core {
	class Harness {
		lv::Instance instance;
		lv::PhysicalDevice* pDevice;
		lv::Device* device;
		std::map<infr::DeviceFunction, std::vector<int>> deviceQueues;
		lv::Surface surface;
		lv::SwapChain swapchain;
		lv::RenderPass renderPass;
		lv::FrameBuffer frameBuffer;
		lv::CommandPool command;

		asset::io::StreamHost host;
		asset::format::Model* lux;

		asset::io::StageVueue stageBuffer;
		asset::io::Vueue vram;
	};
}