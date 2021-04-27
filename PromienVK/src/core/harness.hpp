#include "../lv/lv.hpp"
#include "settings.hpp"
#include "../infr/type.hpp"
#include "../asset/io/loader.hpp"
#include "../asset/format/obj.hpp"

namespace core {
	class Harness {
		conf::Scope configs;
		lv::Instance instance;
		lv::PhysicalDevice* pDevice;
		lv::Device* device;
		std::vector<lv::Queue>* graphicQueues;
		std::vector<lv::Queue>* transferQueues;
		lv::Surface surface;
		lv::SwapChain swapchain;
		lv::RenderPass renderPass;
		lv::FrameBuffer frameBuffer;
		lv::FrameSynchronization synchronization;
		lv::CommandPool command;

		asset::io::StreamHost host;
		asset::format::Model* lux;

		asset::io::StageVueue stageBuffer;
		asset::io::Vueue vram;

	public:
		Harness(const std::string& config);
		void start();
		~Harness();
	};
}