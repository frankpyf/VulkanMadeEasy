#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include <vector>
#include <memory>

namespace vme {
    class VulkanEngine
    {
    public:
        class Builder
        {
        private:
            VkApplicationInfo app_info_{
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO, 
                .pEngineName = "VME", 
                .apiVersion = VK_API_VERSION_1_3
            };
            
            std::vector<const char*> extensions_{};
            std::vector<const char*> layers_{};
        public:
            Builder& SetAppName(const char* app_name);
            // If you don't set api version, the default one will be Vulkan1.3
            Builder& SetApiVersion(uint8_t major, uint8_t minor);
            // All necessary extensions should be included automatically. 
            // Use this function only if you want to add special ones.
            Builder& AddExtension(const char* extension_name);
            Builder& AddLayer(const char* layer_name);

            std::unique_ptr<VulkanEngine> Build();
        };
        struct Context
        {
            VkInstance instance = VK_NULL_HANDLE; // Vulkan library handle
	        VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE; // Vulkan debug output handle
	        VkPhysicalDevice chosen_gpu = VK_NULL_HANDLE; // GPU chosen as the default device
	        VkDevice device = VK_NULL_HANDLE; // Vulkan device for commands
	        VkSurfaceKHR surface = VK_NULL_HANDLE; // Vulkan window surface
        };
    public:
        Context context_;
    private:

    public:
        VulkanEngine(VkInstanceCreateInfo);
        ~VulkanEngine();
    };
}