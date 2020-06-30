#ifndef CONF_H
#define CONF_H
#include <map>
#include <vector>
#include <string>

//here comes my best json impression

namespace conf {
	enum Type {
		ARRAY,
		MAP,
		NUM,
		STRING,
		NILL
	};

	struct Entry {
		bool ref;
		void* content;
		Type type;
		Entry();
		Entry(Entry& entry);
		Entry(Entry&& entry);
		~Entry();
	};

	struct Scope {
		std::map<std::string, Entry> map;
		Entry& operator[](std::string& key);
	};

	void parseConfigs(Scope& scope, std::string& filename);

}


#endif