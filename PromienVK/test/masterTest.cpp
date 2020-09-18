#include "masterTest.hpp"

int masterTest(bool stress) {
	confTest();
	dictTest();
	allocTest(stress);
	heapTest();
	asyncTest();
	return 0;
}