#include <vulkan/vulkan.h>

namespace vme {
    class VulkanCore
    {
    public:
        class Builder
        {
        public:
            Builder& SetAppName(const char* name);
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
        

    public:
        VulkanCore(/* args */);
        ~VulkanCore();
    };
}