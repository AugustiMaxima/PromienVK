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

		void pickDevices(map<string, vector<PhysicalDevice>>& map, SurfaceKHR surface) {
			vector<PhysicalDevice>& dlst = map["*"];
			for (auto& device : dlst) {
				infr::dvs::registerDeviceSet(map, device);
				if (presentReady(device)) {
					map["present"].push_back(device);
				}
			}

			infr::dvs::rankDeviceEligibility(map);

			auto& pp = map["present"];
			std::sort(pp.begin(), pp.end(), presentScore);
		}
	}
}