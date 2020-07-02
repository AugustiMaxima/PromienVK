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

	struct Entry;

	struct Scope {
		std::map<std::string, Entry> map;
		Entry& operator[](std::string key);
	};
	
	struct Entry {
		void* content;
		Type type;
		Entry();
		Entry(const Entry& entry);
		Entry(Entry&& entry);
		Entry& operator=(Entry& entry);
		Entry& operator=(Entry&& entry);
		operator std::string& ();
		operator int& ();
		operator Scope();
		operator Scope& ();
		operator std::vector<Entry> ();
		operator std::vector<Entry>& ();
		Entry& operator=(int num);
		Entry& operator=(std::string str);
		Entry& operator=(Scope& scp);
		Entry& operator=(std::vector<Entry>& arr);
		~Entry();
	private:
		bool ref;
		void gc();
	};



	void parseConfigs(Scope& scope, std::string filename);

}


#endif