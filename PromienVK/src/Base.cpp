#include "Base.hpp"

namespace infr {
	Base::Base(std::string config) :InfraVK() {
		conf::parseConfigs(configs, config);
	}

	Base::~Base() {
		//TO BE COMPLETED
	}



}