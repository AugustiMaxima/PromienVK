#ifndef CONF_H
#define CONF_H
#include <map>
#include <vector>
#include <string>

//here comes my best json impression

namespace conf {
	enum class Type {
		ARRAY,
		MAP,
		NUM,
		STRING,
		NILL
	};

	struct Entry {
		void* content;
		Type type;
		Entry();
		Entry(Entry& entry);
		Entry(Entry&& entry);
		Entry& operator=(Entry& entry);
		Entry& operator=(Entry&& entry);
		~Entry();
	private:
		bool ref;
		void gc();
	};

	struct Scope {
		std::map<std::string, Entry> map;
		Entry& operator[](std::string key);
	};

	void parseConfigs(Scope& scope, std::string filename);

}


#endif