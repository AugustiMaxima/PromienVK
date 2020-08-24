#include "hvm.hpp"

namespace infr {
	namespace hvm {

		vNode::vNode(int offset, int bytes, vNode* p, HierarchicalVM& vm):offset(offset), bytes(bytes), p(p) {
			l = nullptr;
			r = nullptr;

			reg = new allocRegistry(this);
			vm.attachRegistry(reg);
		}

		//invariant: requests are made to valid leaf nodes
		vNode* vNode::allocRequest(int size, HierarchicalVM& vm){
			if (bytes > size * 2) {//condition for split
				l = new vNode(offset, bytes / 2, this, vm);
				//invariant here, please let bytes be a multiple of 2
				r = new vNode(offset + bytes / 2, bytes / 2, this, vm);

				//we will defer to picking l, as randomization isnt really useful
				l->allocRequest(size, vm);
			}
			else {
				vm.removeRegistry(reg);
				return this;
			}
		}

		vNode* vNode::findOffset(int displacement, HierarchicalVM& vm) {
			if (l) {//invariant: if one child exist, the other must also exist
				if (displacement < offset + bytes / 2) {
					return l->findOffset(displacement, vm);
				}
				else {
					return r->findOffset(displacement, vm);
				}
			}
			if (displacement >= offset && displacement < offset + bytes)
				return this;
			return nullptr;
		}

		void vNode::purgeLeaf(HierarchicalVM& vm) {
			vm.removeRegistry(reg);
			delete this;
		}

		//invariant: no parent must exist where both child nodes are unallocated leaves
		vNode* vNode::free(HierarchicalVM& vm) {
			if (p) {
				//check sibling
				vNode* sib = p->l == this ? r : l;
				if (sib->l || sib->reg) {
				}
				else {
					sib->purgeLeaf(vm);
					purgeLeaf(vm);
					p->l = nullptr;
					p->r = nullptr;
					return p->free(vm);
				}
			}
			reg = new allocRegistry(this);
			vm.attachRegistry(reg);
			return this;
		}

		vNode::~vNode() {
			delete l;
			delete r;
		}

		allocRegistry::allocRegistry(vNode* node) : node(node), front(nullptr), back(nullptr){}

		void HierarchicalVM::attachRegistry(allocRegistry* reg) {
			auto result = fragmentList.get(reg->node->bytes);
			if (result)
				reg->back = result.value();
			fragmentList.put(reg->node->bytes, reg);
		}
		
		void HierarchicalVM::removeRegistry(allocRegistry* reg) {
			if(reg->back)
				reg->back->front = reg->front;
			if (reg->front) {
				reg->front->back = reg->back;
			}
			else {//front of the pack
				fragmentList.put(reg->node->bytes, reg->back);
				reg->node->reg = nullptr;
			}
			delete reg;
		}

		HierarchicalVM::HierarchicalVM(int maxHeapSize, int minHeapSize, int padding):maxHeapSize(maxHeapSize), minHeapSize(minHeapSize), padding(padding) {
			root = new vNode(0, maxHeapSize, nullptr, *this);
		}
	
		int HierarchicalVM::malloc(int size) {
			if (size < minHeapSize)
				size = minHeapSize;
			auto key = fragmentList.query(size);
			allocRegistry* reg = nullptr;
			if (key) {
				if (key.getKey() < size) {
					key = key++;
				}
				if (key) {
					reg = key;
				}
			}
			if (!reg)
				throw std::exception("Allocation failed");
			removeRegistry(reg);
			return reg->node->offset - reg->node->offset % padding;
		}

		void HierarchicalVM::free(int offset) {
			if (!root)
				return;
			vNode* node = root->findOffset(offset, *this);
			node->free(*this);
		}
	
		HierarchicalVM::~HierarchicalVM() {
			delete root;
			std::vector<allocRegistry*> regs;
			auto k = fragmentList.query(0);
			while (k++) {
				regs.push_back(k);
			}
			for (auto& r : regs) {
				allocRegistry* reg = r;
				allocRegistry* n;
				 while (n = reg->back) {
					delete reg;
					reg = n;
				 }
				 delete reg;
			}
		}
	
	
	}
}