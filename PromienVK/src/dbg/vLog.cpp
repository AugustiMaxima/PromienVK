
#if defined(_DEBUG)
#include "VLog.hpp"

namespace dbg {

    VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallBack(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
        if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        }

        return VK_FALSE;
    }
}


#endif