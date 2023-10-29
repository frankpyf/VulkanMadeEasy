#include <vulkan/vulkan.h>

namespace vme {
    class VulkanCore
    {
    private:
        VkInstance instance_;
        /* data */
    public:
        VulkanCore(/* args */);
        ~VulkanCore();
    };
}