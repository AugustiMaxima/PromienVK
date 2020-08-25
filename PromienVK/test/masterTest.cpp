#include "masterTest.hpp"

int masterTest(bool stress) {
	confTest();
	dictTest();
	allocTest(stress);
	return 0;
}