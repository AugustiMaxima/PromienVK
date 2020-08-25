#ifndef LINEAR_VM_HPP
#define LINEAR_VM_HPP

#include "../utils/multindex.hpp"

//hvm did not have the best ram utilization, time to try something more "aggressive"

namespace infr {
	namespace lvm {
		struct rNode;
		class LinearVM;
		struct mNode {
			int offset;
			int bytes;
			mNode* f;
			mNode* r;
			rNode* reg;
			mNode(int offset, int bytes, LinearVM& vm, bool alloced = false);
			mNode* allocRequest(int size, LinearVM& vm);
			void free(LinearVM& vm);
		};

		struct rNode {
			mNode* node;
			rNode* f;
			rNode* r;
			rNode(mNode* node);
		};

		class LinearVM {
			const int maxHeapSize;
			mNode* src;
			util::multIndex<int, rNode*> freeList;
			util::multIndex<int, mNode*> allocRecord;
			void attachRegistry(rNode* reg);
			void removeRegistry(rNode* reg);
			friend mNode;
		public:
			const int minHeapSize;
			const int align;
			LinearVM(int maxHeapSize, int minHeapSize, int align = 4);
			int malloc(int size);
			void free(int offset);
			~LinearVM();
		};
	}
}

#endif