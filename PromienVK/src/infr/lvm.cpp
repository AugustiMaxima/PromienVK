#include "lvm.hpp"

namespace infr {
	namespace lvm {
		mNode::mNode(uint64_t offset, uint64_t bytes, LinearVM& vm, bool alloced) :offset(offset), bytes(bytes), f(nullptr), r(nullptr) {
			if (alloced)
				reg = nullptr;
			else {
				reg = new rNode(this);
				vm.attachRegistry(reg);
			}
		}

		bool mNode::compatible(uint64_t size, uint64_t align) {
			return alignedSize(size, align) <= bytes;
		}

		uint64_t mNode::alignedSize(uint64_t size, uint64_t align) {
			return (offset % align ? (align - offset % align) : 0) + size;
		}

		mNode* mNode::allocRequest(uint64_t size, LinearVM& vm, uint64_t align) {
			vm.removeRegistry(reg);
			uint64_t fSize = alignedSize(size, align);
			if (fSize < bytes) {
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
			else if (fSize == bytes) {
				return this;
			}
			return nullptr;
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

		LinearVM::LinearVM(){
			src = new mNode(0, 0, *this, true);
		}

		LinearVM::LinearVM(uint64_t maxHeapSize) : maxHeapSize(maxHeapSize){
			src = new mNode(0, 0, *this, true);
			//meme first slot to avoid the assignment problem
			src->r = new mNode(0, maxHeapSize, *this);
			src->r->f = src;
		}

		void LinearVM::initialize(uint64_t maxHeapSize){
			src->r = new mNode(0, maxHeapSize, *this);
			src->r->f = src;
		}

		uint64_t LinearVM::upscale(uint64_t size, uint64_t align) {
			return size % align ? ((size / align) + 1) * align : size;
		}

		//Note on prealigned:
		//align buffer will always guarantee success, at the cost of some fragmentation
		//but most of the time, we can assume that the headers are aligned
		//optimistic alignment scheme will attempt to align without padding, and retry if failed
		uint64_t LinearVM::malloc(uint64_t size, uint64_t align, bool prealigned) {
			rNode* reg = try_alloc(size, align, prealigned);
			if (!reg)
				throw std::exception("Alignment failure");
			 return fin_alloc(reg, size, align);
		}

		rNode* LinearVM::try_alloc(uint64_t size, uint64_t align, bool prealigned) {
			size = upscale(size, align);
			for (uint64_t i = 0; i < 2; i++) {
				if (!prealigned) {
					size += align - 1;
					align = 1;
				}
				auto result = freeList.probe(size, -1);
				rNode* reg = nullptr;
				if (result) {
					reg = result;
				}
				if (!reg)
					throw std::exception("Allocation failed");
				if (!reg->node->compatible(size, align)) {
					prealigned = false;
					continue;
				}
				return reg;
			}
			return nullptr;
		}

		uint64_t LinearVM::fin_alloc(rNode* reg, uint64_t size, uint64_t align) {
			mNode* node = reg->node->allocRequest(size, *this, align);
			uint64_t offset = node->offset % align ? ((node->offset / align) + 1) * align : node->offset;
			allocRecord.put(offset, node);
			return offset;
		}

		void LinearVM::free(uint64_t offset) {
			auto rec = allocRecord.pop(offset);
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