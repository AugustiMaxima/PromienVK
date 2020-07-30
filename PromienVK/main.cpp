#include "test/masterTest.hpp"
#include <stdio.h>
#include <Windows.h>

#include <iostream>
using namespace std;


int main() {

#if defined(_DEBUG)

	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

#endif
	//insert your compiler flag here
	masterTest();

	char a;
	cin >> a;

}