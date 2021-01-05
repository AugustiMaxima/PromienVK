#ifndef STREAM_WORKS_H
#define STREAM_WORKS_H

#include "../../infr/Async.hpp"
#include "loader.hpp"

namespace asset {
	namespace io {
		class streamUnit : public infr::asyc::Job {
			vk::Device device;
			StreamHost& host;
			std::string assetPath;
			vk::BufferUsageFlags usage;
			vk::Fence fence;
		public:
			streamUnit(vk::Device device, StreamHost& host, std::string assetPath, vk::BufferUsageFlags usage);
			virtual void work() override;
			vk::Fence getFence();
			bool transferComplete();
			virtual ~streamUnit();
		};
	}
}


#endif