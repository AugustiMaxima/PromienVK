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
			bool updateHeight() {
				int oh = height;
				int lh = l ? l->height : 0;
				int rh = r ? r->height : 0;
				height = (lh > rh ? lh : rh) + 1;
				if (l) {
					l->p = this;
				}
				if (r) {
					r->p = this;
				}
				return oh == height;
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
			iNode(const K& key, V value, iNode* parent = nullptr) :key(key), value(value), p(parent), height(1), l(nullptr), r(nullptr) {
			}
			iNode* find(const K& key) {
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
			iNode* query(const K& key, int direction = 1) {
				if (key < this->key) {
					if (l)
						return l->query(key, direction);
					else {
						if (direction < 0) {
							return this;
						}
						else {
							return this->right();
						}
					}
				}
				else if (key == this->key) {
					return this;
				}
				else {
					if (r)
						return r->query(key, direction);
					else {
						if (direction > 0) {
							return this;
						}
						else {
							return this->left();
						}
					}
				}
			}
			//A word on put: update as a function is only guaranteed for single indexes, don't mix mult and update
			iNode* put(const K& key, V value) {
				if (key < this->key) {
					if (l)
						l = l->put(key, value);
					else {
						l = new iNode(key, value, this);
					}
				}
				else if (key == this->key) {
					this->value = value;
				}
				else {
					if (r)
						r = r->put(key, value);
					else {
						r = new iNode(key, value, this);
					}
				}
				return balance();
			}
			iNode* insert(const K& key, V value) {
				if (key < this->key) {
					l = l->insert(key, value);
				}
				//important note here:
				//insert is intended for multi sets
				//this does not imply that first key hit is the left most, as this collection is subject to shuffling/rebalancing
				else {
					r = r->insert(key, value);
				}
				return balance();
			}
			iNode* left() {
				if (l)
					return l;
				iNode* ps = p;
				iNode* c = this;
				while (ps && ps->l == c) {
					c = ps;
					ps = ps->p;
				}
				return ps;
			}
			iNode* right() {
				if (r)
					return r;
				iNode* ps = p;
				iNode* c = this;
				while (ps && ps->r == c) {
					c = ps;
					ps = ps->p;
				}
				return ps;
			}
			iNode* remove() {
				iNode* rep = nullptr;
				if (l && l->r) {
					rep = l;
					while (rep->r)
						rep = rep->r;
					iNode* ps = rep->p;
					ps->r = rep->l;
					while (ps != this) {
						if (ps->updateHeight())
							break;
						ps = ps->p;
					}
					rep->l = l;
					rep->r = r;
				}
				else if (r && r->l) {
					rep = r;
					while (rep->l)
						rep = rep->l;
					iNode* ps = rep->p;
					ps->l = rep->r;
					while (ps != this) {
						if (ps->updateHeight())
							break;
						ps = ps->p;
					}
					rep->l = l;
					rep->r = r;
				}
				else if (l) {
					rep = l;
					rep->r = r;
				}
				else if (r) {
					rep = r;
					rep->l = l;
				}
				rep->p = p;
				rep->updateHeight();
				l = nullptr;
				r = nullptr;
				delete this;
				return rep;
			}
			iNode* remove(const K& key) {
				if (key < this->key) {
					l = l->remove(key);
				}
				else if (key == this->key) {
					return remove();
				}
				else {
					r = r->remove(key);
				}
				return balance();
			}
			iNode* remove(iNode* node) {
				iNode* ps = node->p;
				bool null = false;
				if (ps->l == node) {
					ps->l = node->remove();
				}
				else if (ps->r == node) {
					ps->r = node->remove();
				}
				else {
					null = true;
				}
				if (!null) {
					while (ps) {
						if(ps->updateHeight())
							break;
						ps = ps->p;
					}
				}
			}
			K getKey() {
				return key;
			}
			V& getValue() {
				return value;
			}
			~iNode() {
				delete l;
				delete r;
			}
		};

		class iKey {
			iNode* src;
		public:
			iKey(iNode* src) :src(src) {
			}
			operator bool() {
				return src;
			}
			operator V&() {
				return src->getValue();
			}
			iKey operator+(int l){
				iNode* s = src;
				for (int i = 0; i < l; i++)
					s = s->right();
				return iKey(s);
			}
			iKey operator++() {
				src = src->right();
				return *this;
			}
			iKey operator++(int) {
				return ++(*this);
			}
		};

		iNode* root;
	public:
		multIndex() :root(nullptr) {
		}
		void put(K key, V value) {
			root->put(key, value);
		}
		std::optional<V> get(const K& key) {
			iNode* kn = root->find(key);
			if (kn)
				return std::optional<V>(kn->getValue());
			return std::optional<V>();
		}
		std::vector<V> query(const K& left, const K& right) {
			iNode* l = root->query(left);
			//edge, inclusive
			iNode* r = root->query(right, -1);
			//edge, inclusive
			iNode* pb = l;
			if (l) {
				while (pb && pb->getKey() == left) {
					l = pb;
					pb = pb->left();
				}
			}
			if (r) {
				pb = r;
				while (pb && pb->getKey() == right) {
					r = pb;
					pb = pb->right();
				}
				//right most limit, helps with the iterator end logic
				r = r->right();
			}
			std::vector<V> results;
			if (l) {
				for (iNode* i = l; i != r; i = i->right()) {
					results.push_back(i->getValue());
				}
			}
			return results;
		}
		iKey select(const K& key) {
			return iKey(root->find(key));
		}
		
		~multIndex() {
			delete root;
		}
	};

}



#endif