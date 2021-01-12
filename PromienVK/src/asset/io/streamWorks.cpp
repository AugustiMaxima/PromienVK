#include "streamWorks.hpp"

namespace asset {
	namespace io {
		ModelToStage::ModelToStage(StreamHost& host, format::Model& model):host(host), model(model){}

		void ModelToStage::work() {
			const int bufferSize = model.getAttributes() * model.getVerticeCount() * sizeof(float);
			stage = host.allocateStageBuffer(bufferSize);
			//TODO:synchronization
			model.copy(stage.getStageSource());
			stage.flushCache();
		}

		StageVueue& ModelToStage::collectStage() {
			return stage;
		}

		ModelToStage::~ModelToStage(){}

		StagePropagation::StagePropagation(StreamHost& host, vk::Buffer& binding, StageVueue& vueue) :host(host), binding(binding), stage(vueue) {
			vk::Device device = host.getDevice();
			device.createFence(vk::FenceCreateInfo());
		}

		void StagePropagation::work() {
			vk::Device device = host.getDevice();
			Vueue vram = host.allocateVRAM(binding, stage.size);
			stream = host.allocateStream(stage, vram);
			stream.transfer();
		}

		bool StagePropagation::transferComplete() {
			return stream.transferComplete();
		}

		vk::Fence StagePropagation::getFence() {
			return fence;
		}

		StagePropagation::~StagePropagation() {

		}
	}
}