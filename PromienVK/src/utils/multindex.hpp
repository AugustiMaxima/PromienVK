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
		public:
			iNode(K key, V value, iNode* parent = nullptr) :key(key), value(value), p(parent), height(0), l(nullptr), r(nullptr) {
			}
			iNode* find(K key);
			//A word on put: update as a function is only guaranteed for single indexes, don't mix mult and update
			iNode* put(K key, V value);
			iNode* insert(K key, V value);
			iNode* left();
			iNode* right();
			iNode* balance();
			iNode* remove(iNode* node);
			iNode* remove(K key);
			~iNode() {
				delete l;
				delete r;
			}
		};

		class iKey {
			iNode* src;
			friend multIndex<K, V>;
		public:
			iKey(iNode* src) :src(src) {
			}
		};

		iNode* root;
	public:
		multIndex() :root(nullptr) {
		}
		V put(K key, V value);
		std::optional<K> get(K key);
		std::vector<K> query(K left, K right);
		~multIndex() {
			delete root;
		}
	};





}



#endif