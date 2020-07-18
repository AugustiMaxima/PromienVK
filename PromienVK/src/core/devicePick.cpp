#include "../infr/deviceSet.hpp"
#include "devicePick.hpp"

using namespace std;
using namespace vk;


namespace core {
	namespace dps {
		bool presentReady(PhysicalDevice device, SurfaceKHR surface) {
			auto qs = device.getQueueFamilyProperties();
			for (int i = 0; i < qs.size(); i++) {
				if (device.getSurfaceSupportKHR(i, surface)) {
					return true;
				}
			}
			return false;
		}

		int presentScore(PhysicalDevice device1, PhysicalDevice device2) {
			return device1.getMemoryProperties().memoryHeaps[0].size
				+ device1.getProperties().apiVersion
				- device2.getMemoryProperties().memoryHeaps[0].size
				- device2.getProperties().apiVersion;
		}

		void pickPhysicalDevices(map<string, vector<PhysicalDevice>>& map, SurfaceKHR surface) {
			vector<PhysicalDevice>& dlst = map["*"];
			for (auto& device : dlst) {
				infr::dvs::registerDeviceSet(map, device);
				if (presentReady(device, surface)) {
					map["present"].push_back(device);
				}
			}

			infr::dvs::rankDeviceEligibility(map);

			auto& pp = map["present"];
			std::sort(pp.begin(), pp.end(), presentScore);
		}

		//"*" references offsets from the original map
		//"key" references the "*" binding for efficiency
		map<string, vector<int>> naiveSelection(map<string, vector<PhysicalDevice>>& pDeviceMap) {
			map<string, vector<int>> dmp;
			set<int> deviceSet;
			map<PhysicalDevice, int> rMap;

			auto& principal = pDeviceMap["*"];
			for (int i = 0; i < principal.size(); i++) {
				rMap[principal[i]] = i;
			}

			//deviceSet construction logic could be fine tuned in the future
			for (int i = 0; i < 1; i++) {
				deviceSet.insert(rMap[pDeviceMap["compute"][i]]);
				deviceSet.insert(rMap[pDeviceMap["graphic"][i]]);
				deviceSet.insert(rMap[pDeviceMap["present"][i]]);
			}

			auto& dls = dmp["*"];

			for (auto& di : deviceSet) {
				dls.push_back(di);
			}

			map<PhysicalDevice, int> iMap;
			for (int i = 0; i < dls.size(); i++) {
				iMap[principal[dls[i]]] = i;
			}

			for (int i = 0; i < 1; i++) {
				dmp["compute"].push_back(iMap[pDeviceMap["compute"][i]]);
				dmp["graphic"].push_back(rMap[pDeviceMap["graphic"][i]]);
				dmp["present"].push_back(rMap[pDeviceMap["present"][i]]);
			}

			return dmp;
		}

		void pickDevices(map<string, vector<PhysicalDevice>>& pDeviceMap, SurfaceKHR surface, map<string, vector<Device>>& deviceMap, std::function<map<string, vector<int>>(map<string, vector<PhysicalDevice>>&)> logic) {
			auto queryMap = logic(pDeviceMap);


		}

	}
}