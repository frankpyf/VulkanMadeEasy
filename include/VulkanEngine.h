#include "vk_mem_alloc.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include "Window.h"

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
            // All necessary extensions should be included automatically. 
            // Use this function only if you want to add special ones.
            InstanceBuilder& AddExtension(const char* extension_name);
            InstanceBuilder& AddLayer(const char* layer_name);

            [[nodiscard]] std::unique_ptr<VulkanEngine> Build();
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
        std::vector<VkPhysicalDevice> gpus_;

    public:
        VulkanEngine(VkInstanceCreateInfo);
        ~VulkanEngine();

        // Create logical device, swapchain and all the other necessary components
        // Make sure you have already set up the environment, e.g. physical device, if needed
        // you don't need to shutdown the engine manually
        void Init();

        // Retrieve available gpus, engine will select a default one.
        const std::vector<VkPhysicalDevice>& GetAvailGpus() { return gpus_; } const
        // Replace the default gpu
        void SelectGpu(VkPhysicalDevice new_gpu) { context_.chosen_gpu = new_gpu; }
    private:
        void EnumerateGpus();
    };
}