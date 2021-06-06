#include "test/masterTest.hpp"
#include <stdio.h>
#include <Windows.h>
#include "src/core/harness.hpp"
#include <iostream>


int main() {

#if defined(_CONSOLE)

	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

#endif
	//insert your compiler flag here
#if defined(_DEVELOPMENT)
	using namespace std;
	char a;
	masterTest(false);
	cout << "Enter a character to resume"<<endl;
	cin >> a;
#endif

	core::Harness pt{ "configs/settings.json" };
	pt.start();

}