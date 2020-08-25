#include "lvm.hpp"

namespace infr {
	namespace lvm {
		mNode::mNode(int offset, int bytes, LinearVM& vm, bool alloced) :offset(offset), bytes(bytes), f(nullptr), r(nullptr) {
			if (alloced)
				reg = nullptr;
			else {
				reg = new rNode(this);
				vm.attachRegistry(reg);
			}
		}

		mNode* mNode::allocRequest(int size, LinearVM& vm) {
			vm.removeRegistry(reg);
			int fSize = size % vm.align ? (size + vm.align - size % vm.align) : size;
			if (fSize + vm.minHeapSize <= bytes) {
				//split condition
				mNode* nf = new mNode(offset, fSize, vm, true);
				mNode* nr = new mNode(offset + fSize, bytes - fSize, vm);
				nf->r = nr;
				nf->f = f;
				nr->f = nf;
				nr->r = r;
				if(f)
					f->r = nf;
				if(r)
					r->f = nr;
				delete this;
				return nf;
			}
			return this;
		}

		void mNode::free(LinearVM& vm) {
			while (f->reg) {
				offset = f->offset;
				bytes += f->bytes;
				vm.removeRegistry(f->reg);
				mNode* oof = f;
				f = f->f;
				delete oof;
			}
			f->r = this;
			while (r && r->reg) {
				bytes += r->bytes;
				vm.removeRegistry(r->reg);
				mNode* oor = r;
				r = r->r;
				delete oor;

			}
			if(r)
				r->f = this;
			reg = new rNode(this);
			vm.attachRegistry(reg);
		}

		rNode::rNode(mNode* node):node(node), f(nullptr), r(nullptr){}

		void LinearVM::attachRegistry(rNode* reg) {
			auto result = freeList.get(reg->node->bytes);
			if (result) {//attach as the second node, skips the lookup
				rNode* f = result.value();
				reg->r = f->r;
				reg->f = f;
				f->r = reg;
				if(reg->r)
					reg->r->f = reg;
			} else
				freeList.put(reg->node->bytes, reg);
		}

		void LinearVM::removeRegistry(rNode* reg) {
			if (reg->r)
				reg->r->f = reg->f;
			if (reg->f) {
				reg->f->r = reg->r;
			}
			else {
				if (reg->r)
					freeList.put(reg->node->bytes, reg->r);
				else
					freeList.remove(reg->node->bytes);
			}
			reg->node->reg = nullptr;
			delete reg;
		}

		LinearVM::LinearVM(int maxHeapSize, int minHeapSize, int align) :maxHeapSize(maxHeapSize), minHeapSize(minHeapSize < align ? align : minHeapSize), align(align) {
			src = new mNode(0, 0, *this, true);
			//meme first slot to avoid the assignment problem
			src->r = new mNode(0, maxHeapSize, *this);
			src->r->f = src;
		}

		int LinearVM::malloc(int size) {
			size = size < minHeapSize ? minHeapSize : size;
			auto result = freeList.probe(size, -1);
			rNode* reg = nullptr;
			if (result) {
				reg = result;
			}
			if (!reg)
				throw std::exception("Allocation failed");
			mNode* node = reg->node->allocRequest(size, *this);
			allocRecord.put(node->offset, node);
			return node->offset;
		}

		void LinearVM::free(int offset) {
			auto rec = allocRecord.get(offset);
			mNode* node = nullptr;
			if (rec)
				node = rec.value();
			if (!rec)
				throw std::exception("Double free or corruption");
			node->free(*this);
		}

		LinearVM::~LinearVM() {
			mNode* n;
			while (n = src->r) {
				delete src;
				src = n;
			}
			delete src;
			std::vector<rNode*> regs;
			auto k = freeList.probe(0, -1);
			while (k) {
				regs.push_back(k);
				k++;
			}
			for (auto& r : regs) {
				rNode* reg = r;
				rNode* n;
				while (n = reg->r) {
					delete reg;
					reg = n;
				}
				delete reg;
			}
		}
	}
}