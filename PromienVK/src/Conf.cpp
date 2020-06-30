#include <iostream>
#include <fstream>
#include "Conf.hpp"
#include <stdlib.h> 

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

	int skipToDelimiter(string& line, int index, char delimiter) {
		for (int i = 0; i < line.length(); i++) {
			if (line[i] == delimiter)
				return i;
		}
		return -1;
	}

	void dash(ifstream& stream, string& line, int& index) {
		index = skipWhitespace(line, index);
		if (index == -1) {
			getline(stream, line);
			index = skipWhitespace(line, 0);
		}
	}

	Scope* parseConfigMap(ifstream& stream, string& line, int& index) {
		Scope* src = new Scope;
		Scope& scope = *src;
		if (line[index++] != '{') {
			throw exception("Bad config file, expecting brace");
		}
		bool empty = true;
		while (!ios::eofbit) {
			if (empty && line[index++] != ',') {
				throw exception("Missing delimiter ,");
			}
			dash(stream, line, index);
			//key name
			if (line[index] == '}') {
				index++;
				break;
			}
			if (line[index++] != '"') {
				throw exception("Bad config file, expecting quotation");
			}
			int mark = skipToDelimiter(line, index, '"');
			string key = line.substr(index, mark - index);
			index = mark;
			dash(stream, line, index);
			if (line[index++] != ':') {
				throw exception("Bad config file, expecting divider");
			}
			//deals with real entry now
			parseConfigGeneric(scope[key], stream, line, index);
		}
		return src;
	}

	vector<Entry>* parseConfigArray(ifstream& stream, string& line, int& index) {
		vector<Entry>* src = new vector<Entry>;
		vector<Entry>& arr = *src;
		if (line[index++] != '[') {
			throw exception("Bad config file, expecting bracket");
		}
		bool empty = true;
		int i = 0;
		while (true) {
			if (empty && line[index++] != ',') {
				throw exception("Missing delimiter ,");
			}
			dash(stream, line, index);
			if (line[index] == ']') {
				break;
			}
			arr.push_back(Entry());
			parseConfigGeneric(arr[i++], stream, line, index);
		}
		return src;
	}

	int* parseConfigNum(ifstream& stream, string& line, int& index) {
		//if I were more studious, I'd do this properly or look up my implementation in trains
		//but I'm a lazy fuck
		//TODO: do this properly or look up my implementation in trains
		int* num = new int;
		int& n = *num;
		n = 0;
		char d = line[index];
		while (d >= '0' && d <= '9') {
			n = n * 10 + d - '0';
			d = line[++index];
		}
		return num;
	}

	string* parseConfigStr(ifstream& stream, string& line, int& index) {
		if (line[index++] != '"') {
			throw exception("Bad config file, expecting open quote");
		}
		int mark = skipToDelimiter(line, index, '"');
		string* str = new string;
		*str = line.substr(index, mark - index);
		index = mark;
		return str;
	}

	void parseConfigGeneric(Entry& entry, ifstream& stream, string& line, int& index) {
		//indicate that this is the master copy
		entry.ref = false;
		dash(stream, line, index);
		char selector = line[index];
		if (selector == '{') {
			entry.type = MAP;
			entry.content = parseConfigMap(stream, line, index);
		}
		else if (selector == '[') {
			entry.type = ARRAY;
			entry.content = parseConfigArray(stream, line, index);
		}
		else if (selector == '"') {
			entry.type = STRING;
			entry.content = parseConfigStr(stream, line, index);
		}
		else if (selector <= '9' && selector >= '0') {
			entry.type = NUM;
			entry.content = parseConfigNum(stream, line, index);
		}
		else {
			throw exception("Cannot parse entry");
		}
	}

	void parseConfigs(Scope& scope, string& filename) {
		ifstream file{ filename };
		string line = "";
		int index = 0;
		bool empty = true;
		while (!ios::eofbit) {
			if (empty && line[index++] != ',') {
				throw exception("Missing delimiter ,");
			}
			dash(file, line, index);
			//key name
			if (line[index] == '}') {
				index++;
				break;
			}
			if (line[index++] != '"') {
				throw exception("Bad config file, expecting quotation");
			}
			int mark = skipToDelimiter(line, index, '"');
			string key = line.substr(index, mark - index);
			index = mark;
			dash(file, line, index);
			if (line[index++] != ':') {
				throw exception("Bad config file, expecting divider");
			}
			//deals with real entry now
			parseConfigGeneric(scope[key], file, line, index);
			dash(file, line, index);
		}
	}

	Entry::Entry() {
		ref = false;
	}

	Entry::Entry(Entry& entry):ref(true), content(entry.content), type(entry.type) {
	
	}

	Entry::Entry(Entry&& entry) : ref(entry.ref), content(entry.content), type(entry.type) {

	}

	Entry::~Entry() {
		if (!ref && type != NILL) {
			switch (type) {
				case ARRAY:
					delete (vector<Entry>*)content;
					break;
				case MAP:
					delete (Scope*)content;
					break;
				case NUM:
					delete (int*)content;
					break;
				case STRING:
					delete (string*)content;
					break;
				default:


			}
		}
	}

	Entry& Scope::operator[](std::string& key) {
		if (map.count(key) == 0) {
			map[key].type = NILL;
		}
		return map[key];
	}

}