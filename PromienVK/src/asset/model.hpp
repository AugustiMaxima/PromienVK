#ifndef MODEL_H
#define MODEL_H
#include <string>

namespace asset {
	//just a prototype, we will use the obj convention
	struct Model {
	public:
		Model();
		virtual int getAttributes() = 0;
		virtual int getVerticeCount() = 0;
		virtual int getPolygonCount() = 0;
		virtual void copy(void* vertex, void* indice) = 0;
		//this is the worse option of the two
		virtual void copy(void* vertex) = 0;
		virtual ~Model();
	};
}

#endif