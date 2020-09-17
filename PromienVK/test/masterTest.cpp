#include "masterTest.hpp"

int masterTest(bool stress) {
	confTest();
	dictTest();
	allocTest(stress);
	asyncTest();
	return 0;
}