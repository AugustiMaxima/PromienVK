#include "deviceLib.hpp"
#include <vector>
#include <map>
#include <functional>
#include "../infr/type.hpp"
#include "Instance.hpp"

namespace lvm {
	std::map<infr::DeviceFunction, std::vector<int>> getSuitableMatches(
		const std::map<infr::DeviceFunction, std::function<int(lv::Instance& instance, lv::Surface& surface)>>& rankMap,
		lv::Instance& instance, lv::Surface& surface) {
		std::map<infr::DeviceFunction, std::vector<int>> index;
		return index;
	}
}