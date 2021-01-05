#ifndef OBJ_H
#define OBH_H

#include "model.hpp"

namespace asset {
	namespace format {
		//there are obj files with just triangle lists and obj with indexes
		//this prototypes the first kind

		//note: simple obj aren't really optimized for redundancy free io
		//this is something we can deal with later when we design our own model format :)
		struct obj : public Model {
			const std::string fileName;
			int attributes;
			//The pattern is that the Models hold the minimum necessary content in order to retrieve from disk
			//creating a hierarchy of disk => (staging) ram => vram
			//we are not going to do that here because objs are not optimized for performance and should not be used in a production build
			//so we will prioritize load time since its likely just a test model
			std::vector<float> vertices;
			std::vector<float> normals;
			std::vector<float> coords;
			std::vector<float> faces;
		public:
			obj(const std::string& fileName);
			virtual int getAttributes() = 0;
			virtual int getVerticeCount() = 0;
			virtual int getPolygonCount() = 0;
			virtual void copy(void* vertex, void* indice) = 0;
			virtual void copy(void* vertex) = 0;
			virtual ~obj();
		};
	}
}

#endif