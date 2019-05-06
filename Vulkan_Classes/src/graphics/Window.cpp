#include "Window.h"

Window::Window(int width, int height)
	:m_Width(width), m_Height(height)
{
	glfwInit(); //<- glfwInit() is called here! And not in CreateGLFWwindow()
	CreateInstance();
	CreateDevice();
	CreateGLFWwindow();
	CreateSurface();
	CreateSwapChain();
	CreateSwapChainImageViews();
}

Window::~Window()
{
	DestroySwapChainImageViews();
	DestroySwapChain();
	DestroySurface();
	DestroyGLFWwindow();
	DestroyDevice();
	DestroyInstance();
}

void Window::CreateInstance()
{
	m_AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	m_AppInfo.pNext = nullptr;
	m_AppInfo.pApplicationName = "Vulkan - Test";
	m_AppInfo.applicationVersion = VK_API_VERSION_1_1;
	m_AppInfo.pEngineName = "Vulkan Test Engine";
	m_AppInfo.engineVersion = VK_API_VERSION_1_1;
	m_AppInfo.apiVersion = VK_API_VERSION_1_1;

	vkEnumerateInstanceLayerProperties(&m_Inst_LayerCount, nullptr);
	m_Inst_LayerProp.resize(m_Inst_LayerCount);
	vkEnumerateInstanceLayerProperties(&m_Inst_LayerCount, m_Inst_LayerProp.data());
	//PrintAllVkLayerProperties(m_Inst_LayerProp.data(), m_Inst_LayerCount);

	vkEnumerateInstanceExtensionProperties(nullptr, &m_Inst_ExtCount, nullptr);
	m_Inst_ExtProp.resize(m_Inst_ExtCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &m_Inst_ExtCount, m_Inst_ExtProp.data());
	//PrintAllVkExtensionProperties(m_Inst_ExtProp.data(), m_Inst_ExtCount);

	//GLFW Required Extentions---------------------------
	uint32_t GLFW_EXT_Count;
	const char** GLFW_EXT = glfwGetRequiredInstanceExtensions(&GLFW_EXT_Count);
	for (int i = 0; i < static_cast<int32_t>(GLFW_EXT_Count); i++)
	{
		m_Inst_ExtStrings.push_back(GLFW_EXT[i]);
	}
	//---------------------------------------------------

	m_InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	m_InstanceInfo.pNext = nullptr;
	m_InstanceInfo.flags = 0;
	m_InstanceInfo.pApplicationInfo = &m_AppInfo;
	m_InstanceInfo.enabledLayerCount = static_cast<uint32_t>(m_Inst_LayersStrings.size());
	m_InstanceInfo.ppEnabledLayerNames = m_Inst_LayersStrings.data();
	m_InstanceInfo.enabledExtensionCount = static_cast<uint32_t>(m_Inst_ExtStrings.size());
	m_InstanceInfo.ppEnabledExtensionNames = m_Inst_ExtStrings.data();

	VkResult result = vkCreateInstance(&m_InstanceInfo, nullptr, &m_Instance);
	VkAssert(result);
}

void Window::DestroyInstance()
{
	vkDestroyInstance(m_Instance, nullptr);
}

void Window::CreateDevice()
{
	vkEnumeratePhysicalDevices(m_Instance, &m_DeviceCount, nullptr);
	m_PhysicalDevices.resize(m_DeviceCount);
	VkResult result = vkEnumeratePhysicalDevices(m_Instance, &m_DeviceCount, m_PhysicalDevices.data());
	VkAssert(result);

	vkGetPhysicalDeviceProperties(*m_PhysicalDevices.data(), &m_DeviceProperties);
	PrintVkPhysicalDeviceProperties(m_DeviceProperties);

	vkGetPhysicalDeviceFeatures(*m_PhysicalDevices.data(), &m_DeviceFeatures);
	//PrintVkPhysicalDeviceFeatures(m_DeviceFeatures);

	vkGetPhysicalDeviceMemoryProperties(*m_PhysicalDevices.data(), &m_DeviceMemProp);
	//PrintVkPhysicalDeviceMemoryProperties(m_DeviceMemProp);

	vkEnumerateDeviceLayerProperties(*m_PhysicalDevices.data(), &m_Device_LayerCount, nullptr);
	m_Device_LayerProp.resize(m_Device_LayerCount);
	vkEnumerateDeviceLayerProperties(*m_PhysicalDevices.data(), &m_Device_LayerCount, m_Device_LayerProp.data());
	//PrintAllVkLayerProperties(m_Device_LayerProp.data(), m_Device_LayerCount);

	vkEnumerateDeviceExtensionProperties(*m_PhysicalDevices.data(), nullptr, &m_Device_ExtCount, nullptr);
	m_Device_ExtProp.resize(m_Device_ExtCount);
	vkEnumerateDeviceExtensionProperties(*m_PhysicalDevices.data(), nullptr, &m_Device_ExtCount, m_Device_ExtProp.data());
	//PrintAllVkExtensionProperties(m_Device_ExtProp.data(), m_Device_ExtCount);

	vkGetPhysicalDeviceQueueFamilyProperties(*m_PhysicalDevices.data(), &m_QueueFamilyCount, nullptr);
	m_QueueFamilyProp.resize(m_QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*m_PhysicalDevices.data(), &m_QueueFamilyCount, m_QueueFamilyProp.data());
	//PrintAllVkQueueFamilyProperties(m_QueueFamilyProp.data(), m_QueueFamilyCount);

	m_DeviceQueueCreateInfo.reserve(m_QueueFamilyCount);
	for (int i = 0; i < static_cast<int32_t>(m_QueueFamilyCount); i++)
	{
		VkDeviceQueueCreateInfo temp;
		temp.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		temp.pNext = nullptr;
		temp.flags = 0;
		temp.queueFamilyIndex = i;
		temp.queueCount = m_QueueFamilyProp[i].queueCount; //Check if valid
		temp.pQueuePriorities = m_QueuePriorities;

		m_DeviceQueueCreateInfo.push_back(temp);
	}

	m_DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	m_DeviceCreateInfo.pNext = nullptr;
	m_DeviceCreateInfo.flags = 0;
	m_DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(m_DeviceQueueCreateInfo.size());
	m_DeviceCreateInfo.pQueueCreateInfos = m_DeviceQueueCreateInfo.data();
	m_DeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_Device_LayersStrings.size());
	m_DeviceCreateInfo.ppEnabledLayerNames = m_Device_LayersStrings.data();
	m_DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_Device_ExtStrings.size());
	m_DeviceCreateInfo.ppEnabledExtensionNames = m_Device_ExtStrings.data();
	m_DeviceCreateInfo.pEnabledFeatures = &m_DeviceFeatures;

	result = vkCreateDevice(m_PhysicalDevices[0], &m_DeviceCreateInfo, nullptr, &m_Device); //Pick most powerful device, not necessary the first.
	VkAssert(result);

	m_Queues.resize(m_QueueFamilyCount);
	for (int i = 0; i < static_cast<int32_t>(m_QueueFamilyCount); i++)
	{
		vkGetDeviceQueue(m_Device, m_DeviceQueueCreateInfo[i].queueFamilyIndex, 0, &m_Queues[i]);
	}
}
void Window::DestroyDevice()
{
	vkDestroyDevice(m_Device, nullptr);
}

void Window::CreateGLFWwindow()
{
	bool glfwPresentationSupport = false;
	for (int i = 0; i < static_cast<int32_t>(m_PhysicalDevices.size()); i++)
	{
		for (int j = 0; j < static_cast<int32_t>(m_QueueFamilyCount); j++)
		{
			if (glfwGetPhysicalDevicePresentationSupport(m_Instance, m_PhysicalDevices[i], m_DeviceQueueCreateInfo[j].queueFamilyIndex) == GLFW_TRUE)
			{
				//std::cout << "Physical Device: " << i <<  " Queue Family: " << j << " supports presentation to GLFW!" << std::endl;
				glfwPresentationSupport = true;
			}
		}
	}
	if (!glfwPresentationSupport)
	{
		std::cout << "None the queue families of any of the physical device support presentation to GLFW!" << std::endl;
	}

	m_pfnCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(NULL, "vkCreateInstance");
	m_pfnCreateDevice = (PFN_vkCreateDevice)glfwGetInstanceProcAddress(m_Instance, "vkCreateDevice");
	m_pfnGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)glfwGetInstanceProcAddress(m_Instance, "vkGetDeviceProcAddr");

	if (int error = glfwVulkanSupported())
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_Window = glfwCreateWindow(m_Width, m_Height, m_AppInfo.pApplicationName, nullptr, nullptr);
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);
	}
	else
	{
		std::cout << "Vulkan is not supported by GLFW! Error: " << error << std::endl;
		GLFWAssert(error);
	}
}

void Window::DestroyGLFWwindow()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Window::CreateSurface()
{
	VkResult result = glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);
	VkAssert(result);

	PhysicalDevice_QueueFamilies_Queues temp;
	for (int i = 0; i < static_cast<int32_t>(m_PhysicalDevices.size()); i++)
	{
		for (int j = 0; j < static_cast<int32_t>(m_QueueFamilyCount); j++)
		{
			temp.physicalDevice = i;
			temp.queueFamily = j;
			temp.queues = m_QueueFamilyProp[j].queueCount;
			temp.queueFlags = m_QueueFamilyProp[j].queueFlags;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevices[i], j, m_Surface, &temp.presentSupport);
			temp.SeparateFlags();
			m_QueueGeneralPresentSupport.push_back(temp);
		}
	}

}

void Window::DestroySurface()
{
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
}

void Window::CreateSwapChain()
{
	for (int i = 0; i < static_cast<int32_t>(m_Device_ExtCount); i++)
	{
		if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, m_Device_ExtProp[i].extensionName))
		{
			//std::cout << "Vulkan SwapChain supported!" << std::endl;
			m_SwapChainEXTAvailable = true;
			break;
		}
	}

	if (!m_SwapChainEXTAvailable)
	{
		std::cout << "Vulkan SwapChain not supported!" << std::endl;
		return;
	}
	else
	{
		//Checks details of the SwapChain
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevices[0], m_Surface, &m_SurfaceCompat);

		vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevices[0], m_Surface, &m_SurfaceFormatCount, nullptr);
		m_SurfaceFormats.resize(m_SurfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevices[0], m_Surface, &m_SurfaceFormatCount, m_SurfaceFormats.data());

		vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevices[0], m_Surface, &m_SurfacePresentModeCount, nullptr);
		m_SurfacePresentModes.resize(m_SurfacePresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevices[0], m_Surface, &m_SurfacePresentModeCount, m_SurfacePresentModes.data());

		m_UsableSwapChain = !m_SurfaceFormats.empty() && !m_SurfacePresentModes.empty();
	}

	if (!m_UsableSwapChain)
	{
		std::cout << "Vulkan No suitable SwapChain found!" << std::endl;
		return;
	}
	else
	{
		//Surface Format
		m_ChosenSurfaceFormat = m_SurfaceFormats[0]; //Defaults to using the first one.

		if (m_SurfaceFormats.size() == 1 && m_SurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
			m_ChosenSurfaceFormat = VkSurfaceFormatKHR{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

		for (const auto& chosenSurfaceFormat : m_SurfaceFormats)
		{
			if (chosenSurfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && chosenSurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				m_ChosenSurfaceFormat = chosenSurfaceFormat;
		}

		//Presentation Modes
		for (const auto& chosenPresentMode : m_SurfacePresentModes)
		{
			if (chosenPresentMode == VK_PRESENT_MODE_FIFO_KHR)
				m_ChosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		}

		m_ImageCount = m_SurfaceCompat.minImageCount;
		if (m_SurfaceCompat.maxImageCount > 0 && m_ImageCount > m_SurfaceCompat.maxImageCount)
			m_ImageCount = m_SurfaceCompat.maxImageCount;

		m_SwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		m_SwapChainCreateInfo.surface = m_Surface;
		m_SwapChainCreateInfo.minImageCount = m_ImageCount;
		m_SwapChainCreateInfo.imageFormat = m_ChosenSurfaceFormat.format;
		m_SwapChainCreateInfo.imageColorSpace = m_ChosenSurfaceFormat.colorSpace;
		m_SwapChainCreateInfo.imageExtent = ChooseNewSwapChainExtent();
		m_SwapChainCreateInfo.imageArrayLayers = 1;
		m_SwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		m_SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		m_SwapChainCreateInfo.queueFamilyIndexCount = 0;
		m_SwapChainCreateInfo.pQueueFamilyIndices = nullptr;
		m_SwapChainCreateInfo.preTransform = m_SurfaceCompat.currentTransform;
		m_SwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		m_SwapChainCreateInfo.presentMode = m_ChosenPresentMode;
		m_SwapChainCreateInfo.clipped = VK_TRUE;
		m_SwapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(m_Device, &m_SwapChainCreateInfo, nullptr, &m_SwapChain);
		VkAssert(result);

		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &m_ImageCount, nullptr);
		m_SwapChainImages.resize(m_ImageCount);
		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &m_ImageCount, m_SwapChainImages.data());

		m_SwapChainImageFormat = m_ChosenSurfaceFormat.format;
		m_SwapChainExtent = m_SwapChainCreateInfo.imageExtent;
	}
}

void Window::DestroySwapChain()
{
	vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
}

void Window::CreateSwapChainImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());
	for (int i = 0; i < static_cast<int32_t>(m_SwapChainImageViews.size()); i++)
	{
		VkImageViewCreateInfo temp = {};
		temp.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		temp.image = m_SwapChainImages[i];
		temp.viewType = VK_IMAGE_VIEW_TYPE_2D;
		temp.format = m_SwapChainImageFormat;

		temp.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		temp.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		temp.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		temp.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		temp.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		temp.subresourceRange.baseMipLevel = 0;
		temp.subresourceRange.levelCount = 1;
		temp.subresourceRange.baseArrayLayer = 0;
		temp.subresourceRange.layerCount = 1;

		m_SwapChainImageViewCreateInfos.push_back(temp);

		VkResult result = vkCreateImageView(m_Device, &temp, nullptr, &m_SwapChainImageViews[i]);
		VkAssert(result);
	}
}

void Window::DestroySwapChainImageViews()
{
	for (auto imageView : m_SwapChainImageViews)
		vkDestroyImageView(m_Device, imageView, nullptr);
	m_SwapChainImageViewCreateInfos.clear();
}

void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	app->SetWidthHeight(width, height);
	app->m_FramebufferResized = true;
}

VkExtent2D Window::ChooseNewSwapChainExtent()
{
	if (m_SurfaceCompat.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return m_SurfaceCompat.currentExtent;
	else
	{
		int glfwExtentWidth, glfwExtentHeight;
		glfwGetFramebufferSize(m_Window, &glfwExtentWidth, &glfwExtentHeight);

		VkExtent2D actualExtent =
		{
			static_cast<uint32_t>(glfwExtentWidth),
			static_cast<uint32_t>(glfwExtentHeight)
		};

		actualExtent.width = std::max(m_SurfaceCompat.minImageExtent.width, std::min(m_SurfaceCompat.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(m_SurfaceCompat.minImageExtent.height, std::min(m_SurfaceCompat.maxImageExtent.height, actualExtent.height));

		m_Width = actualExtent.width;
		m_Height = actualExtent.height;

		return actualExtent;
	}
}

void Window::Update() //Call inside a loop!
{
	glfwPollEvents();
}