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
			vm.removeRegistry(reg);
			if (bytes/2 >= size) {//condition for split
				l = new vNode(offset, bytes / 2, this, vm);
				//invariant here, please let bytes be a multiple of 2
				r = new vNode(offset + bytes / 2, bytes / 2, this, vm);

				//we will defer to picking l, as randomization isnt really useful
				return l->allocRequest(size, vm);
			}
			else {
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
				vNode* sib = p->l == this ? p->r : p->l;
				if (sib->l || !sib->reg) {
				}
				else {
					sib->purgeLeaf(vm);
					vNode* pr = p;
					delete this;
					pr->l = nullptr;
					pr->r = nullptr;
					return pr->free(vm);
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
			if (result) {
				allocRegistry* back = result.value();
				reg->back = back;
				back->front = reg;
			}
			fragmentList.put(reg->node->bytes, reg);
		}
		
		void HierarchicalVM::removeRegistry(allocRegistry* reg) {
			if(reg->back)
				reg->back->front = reg->front;
			if (reg->front) {
				reg->front->back = reg->back;
			}
			else {//front of the pack
				if (reg->back)
					fragmentList.put(reg->node->bytes, reg->back);
				else
					fragmentList.remove(reg->node->bytes);
			}
			reg->node->reg = nullptr;
			delete reg;
		}

		HierarchicalVM::HierarchicalVM(int maxHeapSize, int minHeapSize, int padding):maxHeapSize(maxHeapSize), minHeapSize(minHeapSize), padding(padding) {
			root = new vNode(0, maxHeapSize, nullptr, *this);
		}
	
		int HierarchicalVM::malloc(int size) {
			if (size < minHeapSize)
				size = minHeapSize;
			auto key = fragmentList.probe(size,-1);
			allocRegistry* reg = nullptr;
			if (key) {
				reg = key;
			}
			if (!reg)
				throw std::exception("Allocation failed");
			vNode* node = reg->node->allocRequest(size, *this);
			return node->offset - node->offset % padding;
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
			auto k = fragmentList.probe(0);
			while (k) {
				regs.push_back(k);
				k++;
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