#include "vk_mem_alloc.h"
#include <unordered_map>
#include <string>
#include <string_view>

#include "Renderer.h"

namespace vme {
    class VulkanEngine
    {
    public:
        class InstanceBuilder
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
            InstanceBuilder& SetAppName(const char* app_name);
            // If you don't set api version, the default one will be Vulkan1.3
            InstanceBuilder& SetApiVersion(uint8_t major, uint8_t minor);
            InstanceBuilder& AddExtension(const char* extension_name);
            InstanceBuilder& AddLayer(const char* layer_name);

            VkResult Build(VulkanEngine&);
        };
        class DeviceBuilder
        {
            VkDeviceCreateInfo create_info_;
            std::vector<const char*> extensions_{};
            std::vector<const char*> layers_{};
        public:

            // TODO: let user set the rule for choosing a gpu

            DeviceBuilder& SelectGpu(VulkanEngine&);
            DeviceBuilder& AddExtension(const char* extension_name);
            DeviceBuilder& AddLayer(const char* layer_name);

            VkResult Build(VulkanEngine&);
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
        VulkanEngine();
        ~VulkanEngine();

        VulkanEngine(const VulkanEngine&) = delete;
        VulkanEngine& operator=(const VulkanEngine&) = delete;

        void Run();

        void CreateInstance(InstanceBuilder&);
        // Default way of building a vulkan instance
        // User just need to fill in the app name
        void CreateInstance(const char* app_name);

        void CreateDevice(DeviceBuilder&);
        // Default way building a vulkan device
        void CreateDevice();
    };
}