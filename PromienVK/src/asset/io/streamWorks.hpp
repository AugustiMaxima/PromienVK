#ifndef STREAM_WORKS_H
#define STREAM_WORKS_H

#include "../../infr/Async.hpp"
#include "../format/model.hpp"
#include "loader.hpp"

namespace asset {
	namespace io {
		class ModelToStage : public infr::asyc::Job {
			StreamHost& host;
			format::Model& model;
			StageVueue stage;
		public:
			ModelToStage(StreamHost& host, format::Model& model);
			virtual void work() override;
			StageVueue& collectStage();
			virtual ~ModelToStage();
		};

		class StagePropagation : public infr::asyc::Job {
			StreamHost& host;
			vk::Buffer& binding;
			StageVueue& stage;
			StreamHandle stream;
			vk::Fence fence;
		public:
			StagePropagation(StreamHost& host, vk::Buffer& binding, StageVueue& vueue);
			virtual void work() override;
			bool transferComplete();
			vk::Fence getFence();
			virtual ~StagePropagation();
		};
	}
}


#endif