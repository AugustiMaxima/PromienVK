#include <fstream>
#include "shader.hpp"

namespace core {
	namespace pipeline {
		using namespace vk;
		using namespace std;
		ShaderModule createShaderModule(vk::Device device, const string& path) {
			ifstream src{ path, ios::ate | ios::binary};
			if (!src.is_open())
				throw std::runtime_error("failed to open file!");
			int size = src.tellg();
			vector<char> binary(size);
			src.seekg(0);
			src.read(binary.data(), size);
			src.close();
			ShaderModuleCreateInfo info = ShaderModuleCreateInfo()
				.setCodeSize(size)
				.setPCode((uint32_t*)binary.data());
			return device.createShaderModule(info);
		}

		PipelineShaderStageCreateInfo createShaderStage(ShaderModule module, ShaderStageFlagBits flag, const string& entry) {
			PipelineShaderStageCreateInfo info = PipelineShaderStageCreateInfo()
				.setModule(module)
				.setPName(entry.c_str())
				.setStage(flag);
			return info;
		}

	}
}
