#include "../infr/deviceSet.hpp"
#include "devicePick.hpp"

using namespace std;
using namespace vk;

struct featureList {
	int set[3];
	int& operator[](int i) {
		return set[i];
	}
};

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

		void pickDevices(map<string, vector<PhysicalDevice>>& pDeviceMap, SurfaceKHR surface,
			map<string, vector<Device>>& deviceMap, map<string, util::multIndex<float, Queue>>& queueMap,
			function<map<string, vector<int>>(map<string, vector<PhysicalDevice>>&)> logic) {
			auto queryMap = logic(pDeviceMap);
			//as we may have multiple queues demanding from the same physicalDevice, we can use queues from the same device
			//length = number of devices in ["*"], ie selected
			vector<featureList> qSup;
			//as it's possible for even queues to perform multiple functions, we also want to enable queue sharing
			auto& dlist = queryMap["*"];
			qSup.resize(dlist.size());

			//initialization
			for (int i = 0; i < dlist.size(); i++) {
				for (int j = 0; j < 3; j++) {
					qSup[i][j] = -1;
				}
			}
			//we will stick with one queue per function for now
			//compute pass
			for (int pid : queryMap["compute"]) {
				if (qSup[pid][0] == -1) {
					auto& device = pDeviceMap["*"][queryMap["*"][pid]];
					auto qs = device.getQueueFamilyProperties();
					for (int i = 0; i < qs.size(); i++) {
						if (qs[i].queueFlags & vk::QueueFlagBits::eCompute) {
							qSup[pid][0] = i;
						}
					}
				}
			}
			//graphic pass
			for (int pid : queryMap["graphic"]) {
				if (qSup[pid][1] == -1) {
					auto& device = pDeviceMap["*"][queryMap["*"][pid]];
					auto qs = device.getQueueFamilyProperties();
					for (int i = 0; i < qs.size(); i++) {
						if (qs[i].queueFlags & vk::QueueFlagBits::eGraphics) {
							qSup[pid][1] = i;
						}
					}
				}
			}
			//present pass
			for (int pid : queryMap["present"]) {
				if (qSup[pid][2] == -1) {
					auto& device = pDeviceMap["*"][queryMap["*"][pid]];
					auto qs = device.getQueueFamilyProperties();
					for (int i = 0; i < qs.size(); i++) {
						if (device.getSurfaceSupportKHR(i, surface)) {
							qSup[pid][2] = i;
						}
					}
				}
			}

			//consolidations here
			for (int i = 0; i < dlist.size(); i++) {
				auto& device = pDeviceMap["*"][dlist[i]];
				DeviceCreateInfo info{};
				featureList ss = qSup[i];
				set<int> s;
				for (int i = 0; i < 3; i++) {
					if (ss[i] >= 0) {
						s.insert(ss[i]);
					}
				}
				vector<DeviceQueueCreateInfo> deviceQeueues;
				float qp = 1.0f;
				for (auto i : s) {
					DeviceQueueCreateInfo info{};
					info.queueFamilyIndex = i;
					info.queueCount = 1;
					info.pQueuePriorities = &qp;
					deviceQeueues.push_back(info);
				}
				//may extend the deviceFeature in the future
				PhysicalDeviceFeatures deviceFeatures{};
				info.pEnabledFeatures = &deviceFeatures;
				info.pQueueCreateInfos = deviceQeueues.data();
				info.queueCreateInfoCount = deviceQeueues.size();
				info.enabledExtensionCount = 0;
				info.enabledLayerCount = 0;

				Device ld = device.createDevice(info);
				deviceMap["*"].push_back(ld);
			}

			//assumptions here to be changed: each device only get one queue, this may change in the future

			//compute device gathering
			for (int pid : queryMap["compute"]) {
				Device dv = deviceMap["*"][pid];
				deviceMap["compute"].push_back(dv);
				int qi = qSup[pid][0];
				queueMap["compute"].insert(1.0f, dv.getQueue(qi, 0));
			}
			//graphic device gathering
			for (int pid : queryMap["graphic"]) {
				Device dv = deviceMap["*"][pid];
				deviceMap["graphic"].push_back(dv);
				int qi = qSup[pid][0];
				queueMap["graphic"].insert(1.0f, dv.getQueue(qi, 0));
			}
			//present device gathering
			for (int pid : queryMap["present"]) {
				Device dv = deviceMap["*"][pid];
				deviceMap["present"].push_back(dv);
				int qi = qSup[pid][0];
				queueMap["present"].insert(1.0f, dv.getQueue(qi, 0));
			}
		}

	}
}