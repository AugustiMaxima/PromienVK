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

		void pickPhysicalDevices(map<infr::DeviceFunction, vector<PhysicalDevice>>& deviceMap, map<infr::DeviceFunction, DeviceCreateInfo>& templ, SurfaceKHR surface) {
			vector<PhysicalDevice>& dlst = deviceMap[infr::DeviceFunction::all];
			for (auto& device : dlst) {
				if (presentReady(device, surface) && infr::dvs::graphicCompatible(device) && deviceCompatible(device, templ[infr::DeviceFunction::graphic])) {
					deviceMap[infr::DeviceFunction::graphic].push_back(device);
				}
				if (infr::dvs::computeCompatible(device) && deviceCompatible(device, templ[infr::DeviceFunction::compute])) {
					deviceMap[infr::DeviceFunction::compute].push_back(device);
				}
			}
			std::sort(deviceMap[infr::DeviceFunction::graphic].begin(), deviceMap[infr::DeviceFunction::graphic].end(), infr::dvs::graphicRank);
			std::sort(deviceMap[infr::DeviceFunction::compute].begin(), deviceMap[infr::DeviceFunction::compute].end(), infr::dvs::computeRank);
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
		map<infr::DeviceFunction, vector<int>> physicalDeviceIndexing(map<infr::DeviceFunction, vector<PhysicalDevice>>& pDeviceMap) {
			map<infr::DeviceFunction, vector<int>> dMap;
			map<PhysicalDevice, int> rMap;

			auto& principal = pDeviceMap[infr::DeviceFunction::all];
			for (int i = 0; i < principal.size(); i++) {
				rMap[principal[i]] = i;
			}

			for (int i = 0; i < 1; i++) {
				dMap[infr::DeviceFunction::compute].push_back(rMap[pDeviceMap[infr::DeviceFunction::compute][i]]);
				dMap[infr::DeviceFunction::graphic].push_back(rMap[pDeviceMap[infr::DeviceFunction::graphic][i]]);
			}

			return dMap;
		}

		map<infr::DeviceFunction, vector<bool>> naiveSelection(map<infr::DeviceFunction, vector<int>>& deviceIndice) {
			map<infr::DeviceFunction, vector<bool>> selected;
			selected[infr::DeviceFunction::graphic].push_back(true);
			return selected;
		}

		void pickDevices(map<infr::DeviceFunction, vector<PhysicalDevice>>& pDeviceMap, SurfaceKHR surface,
			map<infr::DeviceFunction, vector<Device>>& deviceMap, map<infr::DeviceFunction, DeviceCreateInfo>& templ,
			function<map<infr::DeviceFunction, vector<bool>>(map<infr::DeviceFunction, vector<int>>&)> selector) {
			auto queryMap = physicalDeviceIndexing(pDeviceMap);
			auto& dlst = pDeviceMap[infr::DeviceFunction::all];
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
			map<infr::DeviceFunction, vector<PhysicalDevice>> npDeviceMap;
			for (auto& id : selected[infr::DeviceFunction::graphic]) {
				Device dvc = allocateDeviceQueue(dlst[id], templ[infr::DeviceFunction::graphic]);
				deviceMap[infr::DeviceFunction::all].push_back(dvc);
				npDeviceMap[infr::DeviceFunction::all].push_back(dlst[id]);
				deviceMap[infr::DeviceFunction::graphic].push_back(dvc);
				npDeviceMap[infr::DeviceFunction::graphic].push_back(dlst[id]);
			}
			for (auto& id : selected[infr::DeviceFunction::compute]) {
				Device dvc = allocateDeviceQueue(dlst[id], templ[infr::DeviceFunction::compute]);
				deviceMap[infr::DeviceFunction::all].push_back(dvc);
				npDeviceMap[infr::DeviceFunction::all].push_back(dlst[id]);
				deviceMap[infr::DeviceFunction::compute].push_back(dvc);
				npDeviceMap[infr::DeviceFunction::compute].push_back(dlst[id]);
			}
			pDeviceMap = npDeviceMap;
		}

		Device allocateDeviceQueue(PhysicalDevice physicalDevice, DeviceCreateInfo templat,
			map<infr::QueueFunction, function<bool(QueueFamilyProperties)>>& query) {
			vector<DeviceQueueCreateInfo> queueInfos;
			vector<vector<float>> priorities;
			map<infr::QueueFunction, bool> fill;
			vector<QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();
			
			for (const auto& entry : query) {
				fill[entry.first] = false;
			}
			
			for (int i = 0; i < queueFamilies.size(); i++) {
				for (const auto& entry : query) {
					if (!fill[entry.first] && entry.second(queueFamilies[i])) {
						vector<float> pri(queueFamilies[i].queueCount);
						for (auto& fp : pri)
							fp = 1.0f;
						priorities.push_back(pri);
						queueInfos.push_back(DeviceQueueCreateInfo()
							.setQueueFamilyIndex(i)
							.setQueueCount(queueFamilies[i].queueCount)
							.setPQueuePriorities(priorities[priorities.size() - 1].data()));
						fill[entry.first] = true;
						break;
					}
				}
			}
			templat.pQueueCreateInfos = queueInfos.data();
			templat.queueCreateInfoCount = queueInfos.size();
			return physicalDevice.createDevice(templat);
		}

		void retrieveQueues(vector<Device>& devices, vector<PhysicalDevice>& deviceRef,
			map<infr::QueueFunction, function<bool(QueueFamilyProperties)>>& query,
			map<Device, map<infr::QueueFunction, util::multIndex<float, Queue>>>& queueMap) {
			for (int i = 0; i < devices.size(); i++) {
				queueMap[devices[i]] = collectDeviceQueue(devices[i], deviceRef[i], query);
			}
		}

		map<infr::QueueFunction, util::multIndex<float, Queue>> collectDeviceQueue(Device device, PhysicalDevice deviceRef,
			map<infr::QueueFunction, function<bool(QueueFamilyProperties)>>& query) {
			map<infr::QueueFunction, util::multIndex<float, Queue>> qMap;
			map<infr::QueueFunction, bool> fill;
			vector<QueueFamilyProperties> qs = deviceRef.getQueueFamilyProperties();

			for (const auto& entry : query) {
				fill[entry.first] = false;
			}

			for (int i = 0; i < qs.size(); i++) {
				for (const auto& entry : query) {
					if (!fill[entry.first] && entry.second(qs[i])) {
						for (int j = 0; j < qs[i].queueCount; j++)
						qMap[entry.first].insert(1.0f, device.getQueue(i, j));
						break;
					}
				}
			}
			return qMap;
		}
	}
}