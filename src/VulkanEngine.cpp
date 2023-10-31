#include "VulkanEngine.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

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
	
    VulkanEngine::VulkanEngine(/* args */)
    {
        glfwSetErrorCallback(
            [](int error, const char* description)
            {
                fprintf(stderr, "Glfw Error %d: %s\n", error, description);
            }
        );
		if (!glfwInit())
			return;
        if (!glfwVulkanSupported())
		{
			fprintf(stderr, "GLFW: Vulkan not supported!\n");
			return;
		}
    }
    
    VulkanEngine::~VulkanEngine()
    {
        glfwTerminate();
    }

    void VulkanEngine::GetExtensions(std::vector<const char*>& in_extensions)
	{
		uint32_t extensions_count = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
		for (uint32_t i = 0; i < extensions_count; ++i)
		{
			in_extensions.emplace_back(extensions[i]);
		}
#ifdef DEBUG
		instance_extensions_.emplace_back("VK_EXT_debug_report");
#endif // DEBUG
	}
    
} // namespace vme
