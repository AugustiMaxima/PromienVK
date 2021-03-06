#include <iostream>
#include "../src/infr/Conf.hpp"
#include "masterTest.hpp"

using namespace std;
using namespace conf;

static int confTest1() {
	Scope scope;
	parseConfigs(scope, "test/testFiles/test.json");
	Scope& glossary = scope["glossary"];
	Scope glossDiv = glossary["GlossDiv"];
	Scope& glossList = glossDiv["GlossList"];
	Scope glossEntry = glossList["GlossEntry"];
	string glossTerm = glossEntry["GlossTerm"];
	Scope glossDef = glossEntry["GlossDef"];
	vector<Entry> glossSeeAlso = glossDef["GlossSeeAlso"];

	if (glossTerm == "Standard Generalized Markup Language") {
		return 0;
	}
	else {
		throw exception("Content mismatch");
	}
}

static int confTest2() {
	Scope scope;
	scope["Super"] = "Man";
	scope["Num"] = 1;
	Scope s2;
	s2["SSD"] = 480;
	scope["sc"] = s2;
	vector<Entry> vv{ 2 };
	vv[0] = 0;
	vv[1] = "1";
	scope["ar"] = vv;
	return 0;
}

static int confTest3() {
	Scope scope;
	parseConfigs(scope, "test/testFiles/test2.json");
	Scope scope2;
	parseConfigs(scope2, "configs/settings.json");

	return 0;

}

int confTest() {
	confTest1();
	confTest2();
	confTest3();
	return 0;
}
