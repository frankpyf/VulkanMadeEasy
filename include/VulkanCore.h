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
    private:
        VkInstance instance_; // Vulkan library handle
	    VkDebugUtilsMessengerEXT debug_messenger_; // Vulkan debug output handle
	    VkPhysicalDevice chosen_gpu_; // GPU chosen as the default device
	    VkDevice device_; // Vulkan device for commands
	    VkSurfaceKHR surface_; // Vulkan window surface

    public:
        VulkanCore(/* args */);
        ~VulkanCore();
    };
}