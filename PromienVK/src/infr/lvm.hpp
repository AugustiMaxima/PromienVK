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
			bool compatible(int size, int align);
			int alignedSize(int size, int align);
			mNode* allocRequest(int size, LinearVM& vm, int align);
			void free(LinearVM& vm);
		};

		struct rNode {
			mNode* node;
			rNode* f;
			rNode* r;
			rNode(mNode* node);
		};

		class LinearVM {
			int maxHeapSize;
			mNode* src;
			util::multIndex<int, rNode*> freeList;
			util::multIndex<int, mNode*> allocRecord;
			void attachRegistry(rNode* reg);
			void removeRegistry(rNode* reg);
			friend mNode;
		public:
			LinearVM();
			LinearVM(int maxHeapSize);
			void initialize(int maxHeapSize);
			int malloc(int size, int align = 4, bool prealigned = true);
			rNode* try_alloc(int size, int align = 4, bool prealigned = true);
			int fin_alloc(rNode* node, int size, int align = 4);
			void free(int offset);
			~LinearVM();
		};
	}
}

#endif