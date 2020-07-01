#include <iostream>
#include "../src/Conf.hpp"
#include "masterTest.hpp"

using namespace std;

using namespace conf;

int confTest() {
	Scope scope;
	
	parseConfigs(scope, "test/test.json");
	
	Scope& glossary = *(Scope*)(scope["glossary"].content);

	Scope& glossDiv = *(Scope*)(glossary["GlossDiv"].content);
	
	Scope& glossList = *(Scope*)(glossDiv["GlossList"].content);
	
	Scope& glossEntry = *(Scope*)(glossList["GlossEntry"].content);

	string& glossTerm = *(string*)(glossEntry["GlossTerm"].content);
	
	Scope& glossDef = *(Scope*)(glossEntry["GlossDef"].content);

	vector<Entry>& glossSeeAlso = *(vector<Entry>*)(glossDef["GlossSeeAlso"].content);

	if (glossTerm == "Standard Generalized Markup Language") {
		return 0;
	}
	else {
		throw exception("Content mismatch");
	}
}