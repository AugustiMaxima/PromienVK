#include <iostream>
#include <fstream>
#include "Conf.hpp"

using namespace std;

namespace conf {
	void parseConfigGeneric(Scope& scope, ifstream& stream, string& line, int index);

	int skipWhitespace(string& line, int index) {
		for (int i = index; i < line.length(); i++) {
			if (line[i] == '\r' || line[i] == '\n' || line[i] == '\t' || line[i] == ' ') {

			}
			else {
				return i;
			}
		}
		return -1;
	}

	void parseConfigMap(Scope& scope, ifstream& stream, string& line, int index) {
		if (line[index++] != '{') {
			throw exception("Bad config file");
		}
		while (true) {
			index = skipWhitespace(scope, index);
			if (index == -1) {
				line = stream.getline();
				index = skipWhitespace(line, 0);
			}
		}
	}

	void parseConfigArray(Scope& scope, ifstream& stream, string& line, int index) {

	}

	void parseConfigNum(Scope& scope, ifstream& stream, string& line, int index) {

	}

	void parseConfigStr(Scope& scope, ifstream& stream, string& line, int index) {

	}

	void parseConfigGeneric(Scope& scope, ifstream& stream, string& line, int index) {

	}

	void parseConfigs(Scope& scope, string& filename) {



	}

}}