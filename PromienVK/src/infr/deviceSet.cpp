#include "deviceSet.hpp"

namespace infr {

	//perhaps in the future we'd add RT Queues

	bool computeCompatibile(vk::PhysicalDevice device);
	bool graphicCompatibile(vk::PhysicalDevice device);
	bool presentCompatibile(vk::PhysicalDevice device);


	//It's important to perhaps quantify how device selection is performed
	//We make some fundamental assumptions:
	//The (discrete) gpu with the latest feature set is likely the best
	//One would not buy a new gpu that is worse
	//For compute & graphics, vram is a useful indicator of abilities
	int computeRank(vk::PhysicalDevice device1, vk::PhysicalDevice device2);
	int graphicRank(vk::PhysicalDevice device1, vk::PhysicalDevice device2);
	int presentRank(vk::PhysicalDevice device1, vk::PhysicalDevice device2);


	void registerDeviceSet(std::map<std::string, std::vector<vk::PhysicalDevice>>& pDeviceMap, vk::PhysicalDevice device) {
		if (computeCompatibile(device)) {
			pDeviceMap["compute"].push_back(device);
		}
		if (graphicCompatibile(device)) {
			pDeviceMap["graphic"].push_back(device);
		}
		if (presentCompatibile(device)) {
			pDeviceMap["present"].push_back(device);
		}
	}

	void rankDeviceEligibility(std::map<std::string, std::vector<vk::PhysicalDevice>>& pDeviceMap) {
		auto& computeList = pDeviceMap["compute"];
		auto& graphicList = pDeviceMap["graphic"];
		auto& presentList = pDeviceMap["present"];
	
		std::sort(computeList.begin(), computeList.end(), computeRank);
		std::sort(graphicList.begin(), graphicList.end(), graphicRank);
		std::sort(presentList.begin(), presentList.end(), presentRank);
	}

}