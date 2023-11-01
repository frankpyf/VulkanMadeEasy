#include "VulkanEngine.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "spdlog/spdlog.h"

namespace vme
{  
	VulkanEngine::Builder& VulkanEngine::Builder::SetAppName(const char* name) 
	{ 
		app_info_.pApplicationName = name; 
		return *this; 
	}
    VulkanEngine::Builder& VulkanEngine::Builder::SetApiVersion(uint8_t major, uint8_t minor)
	{ 
		app_info_.apiVersion = VK_MAKE_API_VERSION(0, major, minor, 0); 
		return *this;
	}
	VulkanEngine::Builder& VulkanEngine::Builder::AddExtension(const char* extension)
	{
		extensions_.push_back(extension);
		return *this;
	}

	std::unique_ptr<VulkanEngine> VulkanEngine::Builder::Build()
	{
		VkInstanceCreateInfo instance_info{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
		instance_info.pApplicationInfo = &app_info_;

		// Add Glfw required extensions
		uint32_t extensions_count = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
		for (uint32_t i = 0; i < extensions_count; ++i)
		{
			extensions_.push_back(extensions[i]);
		}

#ifdef DEBUG
		instance_extensions_.emplace_back("VK_EXT_debug_report");
#endif // DEBUG
		// Add VK_LAYER_KHRONOS_validation
		layers_.push_back("VK_LAYER_KHRONOS_validation");

// otherwise you will get VK_ERROR_INCOMPATIBLE_DRIVER when creating the instance on macos
#if defined(__APPLE__) || defined(__MACH__)
		extensions_.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
		extensions_.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		instance_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

		instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions_.size());
		instance_info.ppEnabledExtensionNames = extensions_.size() > 0? extensions_.data() : nullptr;
		instance_info.enabledLayerCount = static_cast<uint32_t>(layers_.size());
		instance_info.ppEnabledLayerNames = layers_.size() > 0? layers_.data() : nullptr;

		return std::make_unique<VulkanEngine>(instance_info);
	}
	
    VulkanEngine::VulkanEngine(VkInstanceCreateInfo instance_info)
    {
        glfwSetErrorCallback(
            [](int error, const char* description)
            {
				spdlog::error("GLFW error {0}: {1}", error, description);
            }
        );
		if (!glfwInit())
			return;
        if (!glfwVulkanSupported())
		{
			spdlog::error("GLFW: Vulkan not supported!");
			// TODO: Error handling
			return;
		}
		// Create Vulkan Instance
		VkResult result = vkCreateInstance(&instance_info, nullptr, &context_.instance);
		if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
		{
			spdlog::error("Cannot find a compatible Vulkan driver (ICD)");
			abort();
		}
		spdlog::info("Instance created");
    }
    
    VulkanEngine::~VulkanEngine()
    {
        glfwTerminate();
		vkDestroyInstance(context_.instance, nullptr);
		spdlog::info("Instance destroyed");
    }
} // namespace vme
