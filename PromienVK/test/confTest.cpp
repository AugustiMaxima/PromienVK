#include <iostream>
#include "../src/Conf.hpp"
#include "masterTest.hpp"

using namespace std;

using namespace conf;

int confTest() {
	Scope scope;
	
	parseConfigs(scope, "test/test.json");
	
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