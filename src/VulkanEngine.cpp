#include "VulkanEngine.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include "spdlog/spdlog.h"

namespace vme
{  
	static VkAllocationCallbacks instance_callback;
	VulkanEngine::InstanceBuilder& VulkanEngine::InstanceBuilder::SetAppName(const char* name) 
	{ 
		app_info_.pApplicationName = name; 
		return *this; 
	}
    VulkanEngine::InstanceBuilder& VulkanEngine::InstanceBuilder::SetApiVersion(uint8_t major, uint8_t minor)
	{ 
		app_info_.apiVersion = VK_MAKE_API_VERSION(0, major, minor, 0); 
		return *this;
	}
	VulkanEngine::InstanceBuilder& VulkanEngine::InstanceBuilder::AddExtension(const char* extension)
	{
		extensions_.push_back(extension);
		return *this;
	}
	VulkanEngine::InstanceBuilder& VulkanEngine::InstanceBuilder::AddLayer(const char* layer)
	{
		layers_.push_back(layer);
		return *this;
	}


	VkResult VulkanEngine::InstanceBuilder::Build(VulkanEngine& in_engine)
	{
		VkInstanceCreateInfo instance_info{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
#if defined(__APPLE__) || defined(__MACH__)
		instance_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
		instance_info.pApplicationInfo = &app_info_;
		instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions_.size());
		instance_info.ppEnabledExtensionNames = extensions_.empty()? nullptr : extensions_.data();
		instance_info.enabledLayerCount = static_cast<uint32_t>(layers_.size());
		instance_info.ppEnabledLayerNames = layers_.empty()? nullptr : layers_.data();

		// Create Vulkan Instance
		VkResult result = vkCreateInstance(&instance_info, nullptr, &in_engine.context_.instance);
		return result;
	}

	////////////////////////////////////////////////////////////////////////

	vme::VulkanEngine::DeviceBuilder& vme::VulkanEngine::DeviceBuilder::SelectGpu(VulkanEngine& in_engine)
	{
		assert(in_engine.context_.instance != VK_NULL_HANDLE);
		uint32_t gpu_count = 0;
		VkResult result = vkEnumeratePhysicalDevices(in_engine.context_.instance, &gpu_count, nullptr);
		if (result != VK_SUCCESS)
		{
			spdlog::error("Failed to enumerate physical devices");
		}
		assert(gpu_count > 0 && "No GPU that support Vulkan is found!");

		std::vector<VkPhysicalDevice> gpus(gpu_count);
		result = vkEnumeratePhysicalDevices(in_engine.context_.instance, &gpu_count, gpus.data());
		if (result != VK_SUCCESS)
		{
			spdlog::error("Failed to enumerate physical devices");
		}
		std::vector<VkPhysicalDevice> discrete_device{};
    	auto query_gpu = [&](VkPhysicalDevice gpu){
        	VkPhysicalDeviceProperties gpu_properties{};
        	vkGetPhysicalDeviceProperties(gpu, &gpu_properties);
			if (gpu_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				discrete_device.push_back(gpu);
			}
    	};

   		for(auto& gpu : gpus)
    	{
        	query_gpu(gpu);
    	}

		in_engine.context_.chosen_gpu = discrete_device.empty() ? gpus[0] : discrete_device[0];
		assert(in_engine.context_.chosen_gpu != VK_NULL_HANDLE && "No proper device found!");

		return *this;
	}

	VulkanEngine::DeviceBuilder& VulkanEngine::DeviceBuilder::AddExtension(const char* extension)
	{
		extensions_.push_back(extension);
		return *this;
	}
	
	VulkanEngine::DeviceBuilder& VulkanEngine::DeviceBuilder::AddLayer(const char* layer)
	{
		layers_.push_back(layer);
		return *this;
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////

    VulkanEngine::VulkanEngine()
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
    }
    
    VulkanEngine::~VulkanEngine()
    {
        glfwTerminate();
		if(context_.instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(context_.instance, nullptr);
			spdlog::info("Instance destroyed");
		}
		
    }

	void VulkanEngine::Run()
	{
		
	}

	void VulkanEngine::CreateInstance(const char* app_name)
	{
		InstanceBuilder builder{};
		builder = builder.SetAppName(app_name);
		// Add Glfw required extensions
		uint32_t extensions_count = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
		for (uint32_t i = 0; i < extensions_count; ++i)
		{
			builder = builder.AddExtension(extensions[i]);
		}
#ifdef DEBUG
		builder = builder.AddExtension("VK_EXT_debug_report");
#endif // DEBUG
		
		// Add VK_LAYER_KHRONOS_validation
		builder = builder.AddLayer("VK_LAYER_KHRONOS_validation");

		// otherwise you will get VK_ERROR_INCOMPATIBLE_DRIVER when creating the instance on macos
#if defined(__APPLE__) || defined(__MACH__)
		builder = builder.AddExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)
						.AddExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif


		auto result = builder.Build(*this);
		if( result == VK_SUCCESS)
		{
			spdlog::info("Instance created");
		}
		else if(result == VK_ERROR_INCOMPATIBLE_DRIVER)
		{
			spdlog::error("Cannot find a compatible Vulkan driver (ICD)");
			abort();
		}
	}

	void VulkanEngine::CreateInstance(InstanceBuilder& in_builder)
	{
		auto result = in_builder.Build(*this);
		if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
		{
			spdlog::error("Cannot find a compatible Vulkan driver (ICD)");
			abort();
		}
		spdlog::info("Instance created");
	}

	void VulkanEngine::CreateDevice()
	{
		assert(context_.device == VK_NULL_HANDLE && "Logical Device already exists");
		//auto result = DeviceBuilder().AddExtension("VK_KHR_swapchain").Build(*this);
	}

	void VulkanEngine::CreateDevice(DeviceBuilder& builder)
	{
		assert(context_.device == VK_NULL_HANDLE);


	}
} // namespace vme
