#include <vulkan/vulkan.h>
#include <vector>

namespace vme {
    class VulkanEngine
    {
    public:
        class Builder
        {
        private:
            VkApplicationInfo app_info_{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO, .pEngineName = "VME"};
            VkInstanceCreateInfo instance_info_{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
            std::vector<const char*> extensions_{};
            std::vector<const char*> layers_{};
        public:
            Builder& SetAppName(const char* app_name);
            Builder& SetApiVersion(uint8_t major, uint8_t minor);
            Builder& AddExtension(const char* extension_name);
        };
        class Context
        {
            VkInstance instance_ = VK_NULL_HANDLE; // Vulkan library handle
	        VkDebugUtilsMessengerEXT debug_messenger_; // Vulkan debug output handle
	        VkPhysicalDevice chosen_gpu_ = VK_NULL_HANDLE; // GPU chosen as the default device
	        VkDevice device_ = VK_NULL_HANDLE; // Vulkan device for commands
	        VkSurfaceKHR surface_ = VK_NULL_HANDLE; // Vulkan window surface
        };
    private:
        

    private:
        // retrieve required extensions and layers
        void GetExtensions(std::vector<const char*>& in_extensions);
    public:
        VulkanEngine(/* args */);
        ~VulkanEngine();
    };
}