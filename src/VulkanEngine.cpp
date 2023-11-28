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

    VkResult VulkanEngine::DeviceBuilder::Build(VulkanEngine& engine)
    {
		assert(engine.context_.chosen_gpu != VK_NULL_HANDLE && "Select a GPU first");
		assert(engine.context_.device == VK_NULL_HANDLE && "Logical Device already created");
		auto gpu = engine.context_.chosen_gpu;

		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		// extensions related
		device_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions_.size());
		device_create_info.ppEnabledExtensionNames = extensions_.empty() ? nullptr : extensions_.data();

		// validation layer
		device_create_info.enabledLayerCount = static_cast<uint32_t>(layers_.size());
		device_create_info.ppEnabledLayerNames = layers_.empty()? nullptr : layers_.data();

		// Setup queue info
		std::vector<VkDeviceQueueCreateInfo> queue_create_info;
		const float queue_priority[] = { 1.0f };
		int gfx_queue_family_index = -1;
		int compute_queue_family_index = -1;
		int transfer_queue_family_index = -1;

		std::vector<VkQueueFamilyProperties> queue_family_props;
		uint32_t queue_count;
		vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, nullptr);
		queue_family_props.resize(queue_count);
		vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, queue_family_props.data());
		spdlog::info("Found {0} Queue Families",queue_family_props.size());
		
		//select device queue family
		for (uint32_t family_index = 0; family_index < queue_family_props.size(); ++family_index)
		{
			const VkQueueFamilyProperties& curr_props = queue_family_props[family_index];
			bool is_valid_queue = false;
			if (curr_props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (gfx_queue_family_index == -1)
				{
					gfx_queue_family_index = family_index;
					is_valid_queue = true;
					spdlog::info("Initializing Gfx Queue with Queue family {0} which has {1} queues", family_index, curr_props.queueCount);
				}
			}

			if (curr_props.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				if (compute_queue_family_index == -1 && gfx_queue_family_index != family_index)
				{
					compute_queue_family_index = family_index;
					is_valid_queue = true;
					spdlog::info("Initializing Compute Queue with Queue family {0} which has {1} queues", family_index, curr_props.queueCount);
				}
			}

			if (curr_props.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				if (transfer_queue_family_index == -1 &&
					(curr_props.queueFlags & VK_QUEUE_GRAPHICS_BIT) != VK_QUEUE_GRAPHICS_BIT &&
					(curr_props.queueFlags & VK_QUEUE_COMPUTE_BIT) != VK_QUEUE_COMPUTE_BIT)
				{
					transfer_queue_family_index = family_index;
					is_valid_queue = true;
					spdlog::info("Initializing Transfer Queue with Queue family {0} which has {1} queues", family_index, curr_props.queueCount);
				}
			}
			if (!is_valid_queue)
			{
				spdlog::warn("Skipping unnecessary Queue Family {0}: {1} queues", family_index, curr_props.queueCount);
				continue;
			}
			int queue_index = queue_create_info.size();
			queue_create_info.resize(queue_index + 1);
			VkDeviceQueueCreateInfo& curr_queue = queue_create_info[queue_index];
			curr_queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			curr_queue.queueFamilyIndex = family_index;
			//curr_queue.queueCount = curr_props.queueCount;
			curr_queue.queueCount = 1;
			curr_queue.pQueuePriorities = queue_priority;
			
		}

		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_info.size());
		device_create_info.pQueueCreateInfos = queue_create_info.data();

		//Create the device
		auto result = vkCreateDevice(gpu, &device_create_info, nullptr, &engine.context_.device);

        return result;
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
		if(context_.device != VK_NULL_HANDLE)
		{
			vkDestroyDevice(context_.device, nullptr);
			spdlog::info("Device destroyed");
		}
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
		auto result = DeviceBuilder().SelectGpu(*this).AddExtension("VK_KHR_swapchain").Build(*this);
	}

	void VulkanEngine::CreateDevice(DeviceBuilder& builder)
	{
		assert(context_.device == VK_NULL_HANDLE);

	}
} // namespace vme
