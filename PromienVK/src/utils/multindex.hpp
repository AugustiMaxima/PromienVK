#ifndef MULTINDEX_H
#define MULTINDEX_H

#include <optional>
#include <vector>

namespace util {
	template<typename K, typename V>
	class multIndex {
		class iNode {
			K key;
			V value;
			int height;
			iNode* p;
			iNode* l;
			iNode* r;
			void updateHeight() {
				int lh = l ? l->height : 0;
				int rh = r ? r->height : 0;
				height = (lh > rh ? lh : rh) + 1;
				if (l) {
					l->p = this;
				}
				if (r) {
					r->p = this;
				}
			}
			iNode* balance() {
				int lh = l ? l->height : 0;
				int rh = r ? r->height : 0;
				height = (lh > rh ? lh : rh) + 1;
				iNode* anchor = this;
				if (lh - rh > 1) {
					lh = l->l ? l->l->height : 0;
					rh = l->r ? l->r->height : 0;
					if (lh > rh) {//"inline"
						anchor = l;
						anchor->p = p;
						l = anchor->r;
						anchor->r = this;
						updateHeight();
						anchor->updateHeight();
					}
					else {//"v"
						anchor = l->r;
						anchor->p = p;
						iNode* lf = l;
						lf->r = anchor->l;
						l = anchor->r;
						anchor->l = lf;
						anchor->r = this;
						updateHeight();
						lf->updateHeight();
						anchor->updateHeight();
					}
				}
				else if (rh - lh > 1) {
					lh = r->l ? r->l->height : 0;
					rh = r->r ? r->r->height : 0;
					if (rh > lh) {
						anchor = r;
						anchor->p = p;
						r = anchor->l;
						anchor->l = this;
						updateHeight();
						anchor->updateHeight();
					}
					else {
						anchor = r->l;
						anchor->p = p;
						r->l = anchor->r;
						anchor->r = r;
						r = anchor->l;
						anchor->l = this;
						updateHeight();
						anchor->r->updateHeight();
						anchor->updateHeight();
					}
				}
				return anchor;
			}


		public:
			iNode(K key, V value, iNode* parent = nullptr) :key(key), value(value), p(parent), height(1), l(nullptr), r(nullptr) {
			}
			iNode* find(K key) {
				if (key < this->key) {
					if (l)
						return l->find(key);
					else
						return l;
				}
				else if (key == this->key) {
					return this;
				}
				else {
					if (r)
						return r->find(key);
					else
						return r;
				}
			}
			//A word on put: update as a function is only guaranteed for single indexes, don't mix mult and update
			iNode* put(K key, V value, V* residual) {
				if (key < this->key) {
					if (l) 
						l = l->put(key, value, residual);
					else {
						l = new iNode(key, value, this);
						*residual = value;
					}
				}
				else if (key == this->key) {
					*residual = this->value;
					this->value = value;
				}
				else {
					if (r)
						r = r->put(key, value, residual);
					else {
						r = new iNode(key, value);
						*residual = value;
					}
				}
				return balance();
			}
			iNode* insert(K key, V value) {

			}
			iNode* left();
			iNode* right();
			iNode* remove(iNode* node);
			iNode* remove(K key);
			V& getValue();
			~iNode() {
				delete l;
				delete r;
			}
		};

		class iKey {
			multIndex* index;
			iNode* src;
			friend multIndex<K, V>;
		public:
			iKey(iNode* src) :src(src) {
			}
			operator V&() {
				return src->getValue();
			}
		};

		iNode* root;
	public:
		multIndex() :root(nullptr) {
		}
		V put(K key, V value);
		std::optional<K> get(K key);
		std::vector<K> query(K left, K right);
		iKey select(K key);
		iKey iterate(int direction);
		~multIndex() {
			delete root;
		}
	};





}



#endif