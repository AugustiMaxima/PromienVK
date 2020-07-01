#include <iostream>
#include <fstream>
#include "Conf.hpp"
#include <stdlib.h> 

using namespace std;

namespace conf {
	void parseConfigGeneric(Entry& entry, ifstream& stream, string& line, int& index);

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
		for (int i = index; i < line.length(); i++) {
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
		while (true) {
			dash(stream, line, index);
			if (line[index] == '}') {
				index++;
				break;
			}
			if (!empty && line[index++] != ',') {
				throw exception("Missing delimiter ,");
			}
			dash(stream, line, index);
			//key name
			if (line[index++] != '"') {
				throw exception("Bad config file, expecting quotation");
			}
			int mark = skipToDelimiter(line, index, '"');
			string key = line.substr(index, mark - index);
			index = mark + 1;
			dash(stream, line, index);
			if (line[index++] != ':') {
				throw exception("Bad config file, expecting divider");
			}
			//deals with real entry now
			parseConfigGeneric(scope[key], stream, line, index);
			empty = false;
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
			dash(stream, line, index);
			if (line[index] == ']') {
				index++;
				break;
			}
			if (!empty && line[index++] != ',') {
				throw exception("Missing delimiter ,");
			}
			dash(stream, line, index);
			arr.push_back(Entry());
			parseConfigGeneric(arr[i++], stream, line, index);
			empty = false;
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
		index = mark + 1;
		return str;
	}

	void parseConfigGeneric(Entry& entry, ifstream& stream, string& line, int& index) {
		dash(stream, line, index);
		char selector = line[index];
		if (selector == '{') {
			entry.type = Type::MAP;
			entry.content = parseConfigMap(stream, line, index);
		}
		else if (selector == '[') {
			entry.type = Type::ARRAY;
			entry.content = parseConfigArray(stream, line, index);
		}
		else if (selector == '"') {
			entry.type = Type::STRING;
			entry.content = parseConfigStr(stream, line, index);
		}
		else if (selector <= '9' && selector >= '0') {
			entry.type = Type::NUM;
			entry.content = parseConfigNum(stream, line, index);
		}
		else {
			throw exception("Cannot parse entry");
		}
	}

	void parseConfigs(Scope& scope, string filename) {
		ifstream file{ filename };
		string line = "";
		int index = 0;
		bool empty = true;

		while (!file.eof() || index<line.length()) {
			dash(file, line, index);
			if (!empty && line[index++] != ',') {
				throw exception("Missing delimiter ,");
			}
			dash(file, line, index);
			//key name
			if (line[index++] != '"') {
				throw exception("Bad config file, expecting quotation");
			}
			int mark = skipToDelimiter(line, index, '"');
			string key = line.substr(index, mark - index);
			index = mark + 1;
			dash(file, line, index);
			if (line[index++] != ':') {
				throw exception("Bad config file, expecting divider");
			}
			//deals with real entry now
			parseConfigGeneric(scope[key], file, line, index);
			empty = false;
		}
	}

	Entry::Entry() {
		ref = false;
	}

	Entry::Entry(Entry& entry):ref(true), content(entry.content), type(entry.type) {
	
	}

	Entry::Entry(Entry&& entry) : ref(entry.ref), content(entry.content), type(entry.type) {
		entry.ref = true;
	}

	Entry& Entry::operator=(Entry& entry) {
		gc();
		ref = true;
		content = entry.content;
		type = entry.type;
		return *this;
	}

	Entry& Entry::operator=(Entry&& entry) {
		bool oref = ref;
		void* ocontent = content;
		Type otype = type;
		ref = entry.ref;
		content = entry.content;
		type = entry.type;
		ref = entry.ref;
		entry.ref = oref;
		entry.content = ocontent;
		entry.type = otype;
		return *this;
	}

	void Entry::gc() {
		if (!ref && type != Type::NILL) {
			switch (type) {
			case Type::ARRAY:
				delete (vector<Entry>*)content;
				break;
			case Type::MAP:
				delete (Scope*)content;
				break;
			case Type::NUM:
				delete (int*)content;
				break;
			case Type::STRING:
				delete (string*)content;
				break;
			default:
				break;
			}
		}
	}

	Entry::~Entry() {
		gc();
	}

	Entry& Scope::operator[](std::string key) {
		if (map.count(key) == 0) {
			map[key].type = Type::NILL;
		}
		return map[key];
	}

}