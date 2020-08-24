#ifndef HIERARCHICAL_VM_HPP
#define HIERARCHICAL_VM_HPP

#include "../utils/multindex.hpp"

/*
Some forewords here
Although real time graphics are very much real time,
it is much more often that they are constrained by vram budget limit than they are by allocation time
This implies a priority of minimizing segmentation over worst case runtime (we still want good well behaved expected runtime)
This renders TLSF, which would have been our first choice, undesirable 
	-- despite its excellent worst case runtime and internal segmentation, it suffers from external segmentation
The traditional approach, which demolishes external segmentation, is paging, which is unavailable in gpu due to the lack of mmu
We attempt a novel approach to memory mapping, here by known as Hierarchical Virtual Memory or HVM for short
It partitions memory into hierarchical divisions of 2s, guaranteeing internal segmentation under 1/2, with average of 3/4
It paritions memory only on a per need basis, and will recover memory when it is appropriate
Worst case runtime behavior is O(log n), where n = total memory size / allocation size
In practise, a heavily segmented/allocated memory map should yield results closer to O(1)
*/

namespace infr {
	namespace hvm {

		struct allocRegistry;
		struct HierarchicalVM;

		struct vNode {
			int offset;
			int bytes;
			vNode* l;
			vNode* r;
			vNode* p;
			allocRegistry* reg; // nullptr => claimed
			vNode(int offset, int bytes, vNode* p, HierarchicalVM& vm);
			vNode* allocRequest(int size, HierarchicalVM& vm);
			vNode* findOffset(int displacement, HierarchicalVM& vm);
			void purgeLeaf(HierarchicalVM& vm);
			vNode* free(HierarchicalVM& vm);
			~vNode();
		};

		struct allocRegistry {
			vNode* node;
			allocRegistry* front;
			allocRegistry* back;
			allocRegistry(vNode* node);
		};

		struct HierarchicalVM {
			int maxHeapSize;
			int minHeapSize;
			int padding;
			vNode* root;
			//exclusive mdoe, use put and not insert
			util::multIndex<int, allocRegistry*> fragmentList;
			HierarchicalVM(int maxHeapSize, int minHeapSize, int padding = 4);
			void attachRegistry(allocRegistry* reg);
			void removeRegistry(allocRegistry* reg);
			int malloc(int size);
			void free(int offset);
			~HierarchicalVM();
		};

	}
}


#endif