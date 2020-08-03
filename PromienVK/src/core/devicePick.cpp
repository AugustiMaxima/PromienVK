#include "../infr/deviceSet.hpp"
#include "devicePick.hpp"

using namespace std;
using namespace vk;

struct featureList {
	int set[2];
	int& operator[](int i) {
		return set[i];
	}
};

namespace core {
	namespace dps {
		bool presentReady(PhysicalDevice device, SurfaceKHR surface) {
			auto qs = device.getQueueFamilyProperties();
			bool surport = false;
			for (int i = 0; i < qs.size(); i++) {
				if (device.getSurfaceSupportKHR(i, surface)) {
					surport = true;
					break;
				}
			}
			vector<SurfaceFormatKHR> fmt = device.getSurfaceFormatsKHR(surface);
			vector<PresentModeKHR> pm = device.getSurfacePresentModesKHR(surface);
			return surport & fmt.size() & pm.size();
		}

		bool deviceCompatible(PhysicalDevice device, DeviceCreateInfo spec) {
			set<string> rexts{};
			for (int i = 0; i < spec.enabledExtensionCount; i++) {
				rexts.insert(spec.ppEnabledExtensionNames[i]);
			}
			for (const auto& ext : device.enumerateDeviceExtensionProperties()) {
				rexts.erase(ext.extensionName);
			}
			set<string> rlyrs{};
			for (int i = 0; i < spec.enabledLayerCount; i++) {
				rlyrs.insert(spec.ppEnabledLayerNames[i]);
			}
			for (const auto& lyr : device.enumerateDeviceLayerProperties()) {
				rlyrs.erase(lyr.layerName);
			}
			//TODO: Device feature matching
			return rexts.empty();
		}

		void pickPhysicalDevices(map<string, vector<PhysicalDevice>>& deviceMap, map<string, DeviceCreateInfo>& templ, SurfaceKHR surface) {
			vector<PhysicalDevice>& dlst = deviceMap["*"];
			for (auto& device : dlst) {
				if (presentReady(device, surface) && infr::dvs::graphicCompatible(device) && deviceCompatible(device, templ["graphic"])) {
					deviceMap["graphic"].push_back(device);
				}
				if (infr::dvs::computeCompatible(device) && deviceCompatible(device, templ["compute"])) {
					deviceMap["compute"].push_back(device);
				}
			}
			std::sort(deviceMap["graphic"].begin(), deviceMap["graphic"].end(), infr::dvs::graphicRank);
			std::sort(deviceMap["compute"].begin(), deviceMap["compute"].end(), infr::dvs::computeRank);
		}

		//merges multiple device dependencies
		deviceCreateEnclosure iconoSynthesis(vector<DeviceCreateInfo>& templs) {
			deviceCreateEnclosure closure;
			for (const auto& templ : templs) {
				for (int i = 0; i < templ.enabledExtensionCount; i++) {
					closure.exts.insert(templ.ppEnabledExtensionNames[i]);
				}
				for (int i = 0; i < templ.enabledLayerCount; i++) {
					closure.lyrs.insert(templ.ppEnabledLayerNames[i]);
				}
				closure.deviceFeatures.alphaToOne |= templ.pEnabledFeatures->alphaToOne;
				closure.deviceFeatures.depthBiasClamp |= templ.pEnabledFeatures->depthBiasClamp;
				//TODO: fill out this cancerous list
				//Probably on a per need basis
			}
			for (const auto& ext : closure.exts) {
				closure.extensions.push_back(ext.c_str());
			}
			for (const auto& lyr : closure.lyrs) {
				closure.layers.push_back(lyr.c_str());
			}
			closure.ref.enabledExtensionCount = closure.extensions.size();
			closure.ref.ppEnabledExtensionNames = closure.extensions.data();
			closure.ref.enabledLayerCount = closure.layers.size();
			closure.ref.ppEnabledLayerNames = closure.layers.data();
			closure.ref.pEnabledFeatures = &closure.deviceFeatures;

			return closure;
		}

		//"*" references offsets from the original map
		//"key" references the "*" binding for efficiency
		map<string, vector<int>> physicalDeviceIndexing(map<string, vector<PhysicalDevice>>& pDeviceMap) {
			map<string, vector<int>> dMap;
			map<PhysicalDevice, int> rMap;

			auto& principal = pDeviceMap["*"];
			for (int i = 0; i < principal.size(); i++) {
				rMap[principal[i]] = i;
			}

			for (int i = 0; i < 1; i++) {
				dMap["compute"].push_back(rMap[pDeviceMap["compute"][i]]);
				dMap["graphic"].push_back(rMap[pDeviceMap["graphic"][i]]);
			}

			return dMap;
		}

		map<string, vector<bool>> naiveSelection(map<string, vector<int>>& deviceIndice) {
			map<string, vector<bool>> selected;
			selected["graphic"].push_back(true);
			return selected;
		}

		void pickDevices(map<string, vector<PhysicalDevice>>& pDeviceMap, SurfaceKHR surface, 
			map<string, vector<Device>>& deviceMap, map<string, DeviceCreateInfo>& templ,
			function<map<string, vector<bool>>(map<string, vector<int>>&)> selector) {
			auto queryMap = physicalDeviceIndexing(pDeviceMap);
			auto& dlst = pDeviceMap["*"];
			auto selected = selector(queryMap);
			
			//As it's clearly evident, we need to keep track of the source physical Device and the resultant logical Device
			//It's not technically required to keep the full list of qualifying PhysicalDevices
			//We have opted right now to create copies
			//A more performant way to go about this would be just clearing the old entries
			//if that kind of portability is desired, it may be a good idea as it reduces memory consumption and bloat
			//As always, we may revisit this topic in the future

			//More notes:
			//Option 1: Keep it string prefix based
			//Technically more flexible. Allows us to have a complete history of the device selection process
			//Technically accumulates unnecessary amount of memory that is not essential for progream execution
			//Prevents / inconveniences attempts to convert the string keys to enum based keys, which also improves performance
			//Option 2: Clearing old entries
			//Less bloat, more light weight, physicalDevice records that are not registered as logicalDevices do not concern us
			//Allows for the enum keys to proceed smoothly
			//More work, unclear if it could bite us in the future

			//Option 2 it is
			map<string, vector<PhysicalDevice>> npDeviceMap;
			for (auto& id : selected["graphic"]) {
				Device dvc = allocateDeviceQueue(dlst[id], templ["graphic"]);
				deviceMap["*"].push_back(dvc);
				npDeviceMap["*"].push_back(dlst[id]);
				deviceMap["graphic"].push_back(dvc);
				npDeviceMap["graphic"].push_back(dlst[id]);
			}
			for (auto& id : selected["compute"]) {
				Device dvc = allocateDeviceQueue(dlst[id], templ["compute"]);
				deviceMap["*"].push_back(dvc);
				npDeviceMap["*"].push_back(dlst[id]);
				deviceMap["compute"].push_back(dvc);
				npDeviceMap["compute"].push_back(dlst[id]);
			}
			pDeviceMap = npDeviceMap;
		}

		Device allocateDeviceQueue(PhysicalDevice physicalDevice, DeviceCreateInfo templat) {
			vector<QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();
			vector<DeviceQueueCreateInfo> queueInfos = vector<DeviceQueueCreateInfo>(queueFamilies.size());
			vector<vector<float>> priorities = vector<vector<float>>(queueFamilies.size());
			for (int i = 0; i < queueFamilies.size(); i++) {
				queueInfos[i].queueFamilyIndex = i;
				queueInfos[i].queueCount = queueFamilies[i].queueCount;
				priorities[i].resize(queueFamilies[i].queueCount);
				for (int j = 0; j < queueFamilies[i].queueCount; j++)
					priorities[i][j] = 1.0f;
				queueInfos[i].pQueuePriorities = priorities[i].data();
			}
			templat.pQueueCreateInfos = queueInfos.data();
			templat.queueCreateInfoCount = queueInfos.size();
			return physicalDevice.createDevice(templat);
		}

		void retrieveQueues(vector<Device>& devices, vector<PhysicalDevice>& deviceRef,
			map<Device, map<string, util::multIndex<float, Queue>>>& queueMap) {
			for (int i = 0; i < devices.size(); i++) {
				queueMap[devices[i]] = collectDeviceQueue(devices[i], deviceRef[i]);
			}
		}

		map<string, util::multIndex<float, Queue>> collectDeviceQueue(Device device, PhysicalDevice deviceRef) {
			map<string, util::multIndex<float, Queue>> qMap;
			vector<QueueFamilyProperties> qs = deviceRef.getQueueFamilyProperties();
			for (int i = 0; i < qs.size(); i++) {
				//pattern matching
				string key;
				if (qs[i].queueFlags & QueueFlagBits::eGraphics) {
					//graphics (general) queue
					key = "graphic";
				}
				else if (qs[i].queueFlags & QueueFlagBits::eCompute) {
					//async compute queue
					key = "compute";
				}
				else if (qs[i].queueFlags & QueueFlagBits::eTransfer) {
					//transfer/copy queue
					key = "transfer";
				}
				for (int j = 0; j < qs[i].queueCount; j++)
					qMap[key].insert(1.0f, device.getQueue(i, j));
			}
			return qMap;
		}
	}
}