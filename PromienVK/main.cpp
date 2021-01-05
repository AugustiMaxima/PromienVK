#include "test/masterTest.hpp"
#include <stdio.h>
#include <Windows.h>
#include "src/core/prototype.hpp"
#include <iostream>


int main() {

#if defined(_DEBUG)

	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

#endif
	//insert your compiler flag here
#if defined(_DEBUG)
	using namespace std;
	char a;
	masterTest();
	cout << "Enter a character to resume"<<endl;
	cin >> a;
#endif
	core::Prototype pt{ "configs/settings.json" };
	pt.run();

	

}