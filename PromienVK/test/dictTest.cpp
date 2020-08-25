#include "../src/utils/multIndex.hpp"
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

static string intoString(int i) {
	ostringstream oss;
	oss << i;
	return oss.str();
}

static string ftoString(float i) {
	ostringstream oss;
	oss << i;
	return oss.str();
}

static void debugTree(util::multIndex<int, float>& mi) {
	vector<util::_debug_Tree_Vis> vkk;
	int height, width;
	mi._debug(width, height, vkk, intoString, ftoString);
	int padding = 0;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < padding + 4; j++) {
			cout << "_";
		}
	}
	cout << endl;
	for (auto& ss : vkk) {
		if (ss.k.length() > padding)
			padding = ss.k.length();
		if (ss.v.length() > padding)
			padding = ss.k.length();
	}
	string* gridk = new string[height * width];
	string* gridv = new string[height * width];
	for (auto& ss : vkk) {
		gridk[width * (ss.y - 1) + ss.x - 1] = ss.k;
		gridv[width * (ss.y - 1) + ss.x - 1] = ss.v;
	}
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			string s = gridk[width * j + i];
			//2 spaces as padding, just for precaution
			int prolog = (padding - s.length()) / 2;
			for (int i = 0; i < prolog + 2; i++)
				cout << " ";
			cout << s;
			for (int i = 0; i < padding - prolog + 2; i++)
				cout << " ";
		}
		cout << endl;		
		for (int i = 0; i < width; i++) {
			string s = gridv[width * j + i];
			//2 spaces as padding, just for precaution
			int prolog = (padding - s.length()) / 2;
			for (int i = 0; i < prolog + 2; i++)
				cout << " ";
			cout << s;
			for (int i = 0; i < padding - prolog + 2; i++)
				cout << " ";
		}
		cout << endl;
	}
}

static void multiplicityTest(){
	cout << "Multiplicity Test" << endl;
	util::multIndex<int, float> multMap;
	debugTree(multMap);
	multMap.insert(5, 1.0f);
	debugTree(multMap);
	multMap.insert(5, 1.1f);
	debugTree(multMap);
	multMap.insert(5, 1.2f);
	debugTree(multMap);
	multMap.insert(5, 1.3f);
	debugTree(multMap);
	multMap.insert(5, 1.4f);
	debugTree(multMap);
	multMap.insert(5, 1.5f);
	debugTree(multMap);
	multMap.insert(5, 1.6f);
	debugTree(multMap);
	multMap.insert(5, 1.7f);
	debugTree(multMap);
	auto vr = multMap.query(5, 5);
	for (auto& i : vr)
		cout << i << endl;
}

static void compositeTest() {
	cout << "Composite Test" << endl;
	util::multIndex<int, float> multMap;
	multMap.put(5, 1.0f);
	multMap.put(1, 1.0f);
	multMap.put(5, 1.2f);
	multMap.put(3, 1.0f);
	multMap.put(4, 1.0f);
	multMap.put(2, 1.0f);
	multMap.put(9, 1.3f);
	multMap.put(8, 1.3f);
	multMap.put(0, 1.0f);
	debugTree(multMap);
	auto vr = multMap.query(0, 5);
	for (auto& i : vr)
		cout << i << endl;
}


static void probeTest() {
	util::multIndex<int, float> m;
	for (int i = 0; i < 256; i++) {
		if (i != 127)
			m.put(i, 1);
	}
	auto l = m.probe(127, 1);
	auto r = m.probe(127, -1);
	cout << l.getKey() << endl;
	cout << r.getKey() << endl;
}


int dictTest() {
	multiplicityTest();
	compositeTest();
	probeTest();


	return 0;
}