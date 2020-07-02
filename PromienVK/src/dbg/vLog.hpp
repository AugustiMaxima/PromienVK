#ifndef VLOG_H
#define VLOG_H

#if defined(_DEBUG)

#include <vulkan/vulkan.hpp>
#include <iostream>

namespace dbg {

    VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallBack(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

}

#endif

#endif