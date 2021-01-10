#ifndef LINEAR_VM_HPP
#define LINEAR_VM_HPP

#include "../utils/multindex.hpp"

//hvm did not have the best ram utilization, time to try something more "aggressive"

namespace infr {
	namespace lvm {
		struct rNode;
		class LinearVM;
		struct mNode {
			uint64_t offset;
			uint64_t bytes;
			mNode* f;
			mNode* r;
			rNode* reg;
			mNode(uint64_t offset, uint64_t bytes, LinearVM& vm, bool alloced = false);
			bool compatible(uint64_t size, uint64_t align);
			uint64_t alignedSize(uint64_t size, uint64_t align);
			mNode* allocRequest(uint64_t size, LinearVM& vm, uint64_t align);
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
			util::multIndex<uint64_t, rNode*> freeList;
			util::multIndex<uint64_t, mNode*> allocRecord;
			void attachRegistry(rNode* reg);
			void removeRegistry(rNode* reg);
			friend mNode;
		public:
			LinearVM();
			LinearVM(uint64_t maxHeapSize);
			void initialize(uint64_t maxHeapSize);
			uint64_t malloc(uint64_t size, uint64_t align = 4, bool prealigned = true);
			rNode* try_alloc(uint64_t size, uint64_t align = 4, bool prealigned = true);
			uint64_t fin_alloc(rNode* node, uint64_t size, uint64_t align = 4);
			void free(uint64_t offset);
			~LinearVM();
		};
	}
}

#endif