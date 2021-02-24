#ifndef DEVICE_LIB_HPP
#define DEVICE_LIB_HPP

#include <map>
#include <vector>
#include <functional>

namespace infr{
	enum class DeviceFunction;
}

namespace lv {
	struct Instance;
	struct Surface;
}

namespace lvm {
	std::map<infr::DeviceFunction, std::vector<int>> getSuitableMatches(
		const std::map<infr::DeviceFunction, std::function<int(lv::Instance& instance, lv::Surface& surface)>>& rankMap,
		lv::Instance& instance, lv::Surface& surface);



}

#endif