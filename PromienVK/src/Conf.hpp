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
		STRING
	};

	struct Entry {
		Entry(void* content, Type type);
		Entry(Entry& entry);
		void* content;
		Type type;
		~Entry();
	private:
		bool archival;
	};

	struct Scope {
		std::map<std::string, Entry> entry;
		Entry& operator[](std::string& key);
	};

	void parseConfigs(Scope& scope, std::string& filename);

}


#endif