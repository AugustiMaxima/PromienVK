#ifndef NAIVE_HPP
#define NAIVE_HPP

namespace lv {
	struct Instance;
	struct Surface;
	struct PhysicalDevice;
	struct Device;
}

namespace lvl {

	void configureInstance(lv::Instance& instance, std::string& appName, uint32_t version);

	void enumeratePhysicalDevices(lv::Instance& instance);

	void initializeSurface(lv::Instance& instance, lv::Surface& surface, int width, int height, std::string& appName);

	lv::PhysicalDevice* collectRenderingDevice(lv::Instance& instance, lv::Surface& surface);

}


#endif