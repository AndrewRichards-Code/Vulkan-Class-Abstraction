#include "Vulkan.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Vulkan::Vulkan(int width, int height, const VertexAttribBinding& vertexdata, const IndexData& indexdata)
	:m_Width(width), m_Height(height), m_VertexData(vertexdata), m_IndexData(indexdata)
{
	glfwInit(); //<- glfwInit() is called here! And not in CreateGLFWwindow()
	CreateInstance();
	CreateDevice();
	CreateGFLWwindow();
	CreateSurface();
	CreateSwapChain();
	CreateSwapChainImageViews();
	CreateShader();
	CreateRenderPass();
	CreateDescriptorLayout();
	CreateGraphicPipeline();
	CreateCommandPool();
	CreateDepthImage();
	CreateFramebuffers();
	CreateTextureImage();
	CreateTextureImageView();
	CreateTextureSampler();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateUniformBuffer();
	CreateDiscriptorPool();
	CreateDescriptorSets();
	CreateCommandbuffers();
	CreateSemaphoresAndFences();
}

Vulkan::~Vulkan()
{
	DestroySemaphoresAndFences();
	DestroyCommandbuffers(); 
	DestroyDescriptorSets();
	DestroyDescriptionorPool();
	DestroyUniformBuffer();
	DestroyIndexBuffer();
	DestroyVertexBuffer();
	DestroyTextureSampler();
	DestroyTextureImageView();
	DestroyTextureImage();
	DestroyFramebuffers();
	DestroyDepthImage();
	DestroyCommandPool();
	DestroyGraphicPipeline();
	DestroyDescriptorLayout();
	DestroyRenderPass();
	DestroyShader();
	DestroySwapChainImageViews();
	DestroySwapChain();
	DestroySurface();
	DestroyGLFWwindow();
	DestroyDevice();
	DestroyInstance();
}

void Vulkan::CreateInstance() 
{
	m_AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	m_AppInfo.pNext = nullptr;
	m_AppInfo.pApplicationName = "Vulkan 1.1.86.0 - Test";
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

void Vulkan::DestroyInstance()
{
	vkDestroyInstance(m_Instance, nullptr);
}

void Vulkan::CreateDevice()
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
void Vulkan::DestroyDevice()
{
	vkDestroyDevice(m_Device, nullptr);
}

void Vulkan::CreateGFLWwindow()
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

void Vulkan::DestroyGLFWwindow()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Vulkan::CreateSurface()
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

void Vulkan::DestroySurface()
{
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
}

void Vulkan::CreateSwapChain()
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

	if(!m_SwapChainEXTAvailable)
	{
		std::cout << "Vulkan SwapChain not supported!" << std::endl;
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

void Vulkan::DestroySwapChain()
{
	vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
}

void Vulkan::CreateSwapChainImageViews()
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

void Vulkan::DestroySwapChainImageViews()
{
	for (auto imageView : m_SwapChainImageViews) 
		vkDestroyImageView(m_Device, imageView, nullptr);
	m_SwapChainImageViewCreateInfos.clear();
}

void Vulkan::CreateShader()
{
	//Vertex Shader
	std::ifstream fileVert(m_FilepathVert, std::ios::ate | std::ios::binary);
	if (!fileVert.is_open())
	{
		std::cout << "Can not open: " << m_FilepathVert.c_str() << std::endl;
	}
	m_FileSizeVert = static_cast<size_t>(fileVert.tellg());
	m_ShaderBinVert.resize(m_FileSizeVert);
	fileVert.seekg(0);
	fileVert.read(m_ShaderBinVert.data(), m_FileSizeVert);
	fileVert.close();

	//Fragment Shader
	std::ifstream fileFrag(m_FilepathFrag, std::ios::ate | std::ios::binary);
	if (!fileFrag.is_open())
	{
		std::cout << "Can not open: " << m_FilepathFrag.c_str() << std::endl;
	}
	m_FileSizeFrag = static_cast<size_t>(fileFrag.tellg());
	m_ShaderBinFrag.resize(m_FileSizeFrag);
	fileFrag.seekg(0);
	fileFrag.read(m_ShaderBinFrag.data(), m_FileSizeFrag);
	fileFrag.close();

	m_ShaderModCreateInfoVert.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	m_ShaderModCreateInfoVert.codeSize = m_ShaderBinVert.size();
	m_ShaderModCreateInfoVert.pCode = reinterpret_cast<const uint32_t*>(m_ShaderBinVert.data());
	m_ShaderModCreateInfoVert.pNext = nullptr;
	m_ShaderModCreateInfoVert.flags = 0;

	m_ShaderModCreateInfoFrag.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	m_ShaderModCreateInfoFrag.codeSize = m_ShaderBinFrag.size();
	m_ShaderModCreateInfoFrag.pCode = reinterpret_cast<const uint32_t*>(m_ShaderBinFrag.data());
	m_ShaderModCreateInfoFrag.pNext = nullptr;
	m_ShaderModCreateInfoFrag.flags = 0;

	VkResult result = vkCreateShaderModule(m_Device, &m_ShaderModCreateInfoVert, nullptr, &m_ShaderModVert);
	VkAssert(result);

	result = vkCreateShaderModule(m_Device, &m_ShaderModCreateInfoFrag, nullptr, &m_ShaderModFrag);
	VkAssert(result);
};

void Vulkan::DestroyShader()
{
	vkDestroyShaderModule(m_Device, m_ShaderModVert, nullptr);
	vkDestroyShaderModule(m_Device, m_ShaderModFrag, nullptr);
}

void Vulkan::CreateRenderPass()
{
	//Attachment Description
	m_ColourAttachmentDesc.format = m_SwapChainImageFormat;
	m_ColourAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT; //No multisampling
	m_ColourAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //Determines attachment state pre/post-render 
	m_ColourAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	m_ColourAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; //Determines stencil attachment state pre/post-render
	m_ColourAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	m_ColourAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	m_ColourAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //Images to be presented in the swap chain

	m_DepthAttachmentDesc.format = FindDepthFormat();
	m_DepthAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT; //No multisampling
	m_DepthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //Determines attachment state pre/post-render 
	m_DepthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	m_DepthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; //Determines stencil attachment state pre/post-render
	m_DepthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	m_DepthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	m_DepthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //Images to be presented in the swap chain

	//Attachment References
	m_ColourAttachmentRef.attachment = 0; //It's index from the Fragment Shader
	m_ColourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	m_DepthAttachmentRef.attachment = 1; //It's index from the Fragment Shader
	m_DepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//Subpasses
	m_SubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	m_SubpassDesc.colorAttachmentCount = 1;
	m_SubpassDesc.pColorAttachments = &m_ColourAttachmentRef;
	m_SubpassDesc.pDepthStencilAttachment = &m_DepthAttachmentRef;

	//Subpass Dependency
	m_SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	m_SubpassDependency.dstSubpass = 0;
	m_SubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	m_SubpassDependency.srcAccessMask = 0;
	m_SubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	m_SubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	//Render
	std::vector<VkAttachmentDescription> attachments = { m_ColourAttachmentDesc , m_DepthAttachmentDesc };

	m_RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	m_RenderPassCreateInfo.pNext = nullptr;
	m_RenderPassCreateInfo.flags = 0;
	m_RenderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	m_RenderPassCreateInfo.pAttachments = attachments.data();
	m_RenderPassCreateInfo.subpassCount = 1;
	m_RenderPassCreateInfo.pSubpasses = &m_SubpassDesc;
	m_RenderPassCreateInfo.dependencyCount = 1;
	m_RenderPassCreateInfo.pDependencies = &m_SubpassDependency;

	VkResult result = vkCreateRenderPass(m_Device, &m_RenderPassCreateInfo, nullptr, &m_RenderPass);
	VkAssert(result);

}

void Vulkan::DestroyRenderPass()
{
	vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
}

void Vulkan::CreateDescriptorLayout()
{
	VkDescriptorSetLayoutBinding ubo;
	ubo.binding = 0; //Index in Vertex Shader
	ubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo.descriptorCount = 1;
	ubo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; //UBO should goto the Vertex Shader
	ubo.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding sampler;
	sampler.binding = 1; //Index in Fragment Shader
	sampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler.descriptorCount = 1;
	sampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; //Sampler should goto the Vertex Shader
	sampler.pImmutableSamplers = nullptr;

	m_DescSetLayoutBinds.push_back(ubo);
	m_DescSetLayoutBinds.push_back(sampler);

	m_DescSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	m_DescSetLayoutCreateInfo.pNext = nullptr;
	m_DescSetLayoutCreateInfo.flags = 0;
	m_DescSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(m_DescSetLayoutBinds.size()); //Number of VkDescriptorSetLayoutBinding
	m_DescSetLayoutCreateInfo.pBindings = m_DescSetLayoutBinds.data();

	VkResult result = vkCreateDescriptorSetLayout(m_Device, &m_DescSetLayoutCreateInfo, nullptr, &m_DescSetLayout);
	VkAssert(result);

}

void Vulkan::DestroyDescriptorLayout()
{
	vkDestroyDescriptorSetLayout(m_Device, m_DescSetLayout, nullptr);
}

void Vulkan::CreateGraphicPipeline()
{
	//Shader Stages
	m_ShaderStageCreateInfoVert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_ShaderStageCreateInfoVert.stage = VK_SHADER_STAGE_VERTEX_BIT;
	m_ShaderStageCreateInfoVert.module = m_ShaderModVert;
	m_ShaderStageCreateInfoVert.pName = "main";
	m_ShaderStageCreateInfoVert.pNext = nullptr;
	m_ShaderStageCreateInfoVert.flags = 0;
	m_ShaderStageCreateInfoVert.pSpecializationInfo = nullptr;

	m_ShaderStageCreateInfoFrag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_ShaderStageCreateInfoFrag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	m_ShaderStageCreateInfoFrag.module = m_ShaderModFrag;
	m_ShaderStageCreateInfoFrag.pName = "main";
	m_ShaderStageCreateInfoFrag.pNext = nullptr;
	m_ShaderStageCreateInfoFrag.flags = 0;
	m_ShaderStageCreateInfoFrag.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo shaderStages[2] = { m_ShaderStageCreateInfoVert, m_ShaderStageCreateInfoFrag };

	//Vertex Input
	m_PipelineVertInStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_PipelineVertInStateCreateInfo.pNext = nullptr;
	m_PipelineVertInStateCreateInfo.flags = 0;
	m_PipelineVertInStateCreateInfo.vertexBindingDescriptionCount = m_VertexData.vertexBindingDescriptionCount; //Describe the VBO structure
	m_PipelineVertInStateCreateInfo.pVertexBindingDescriptions = m_VertexData.pVertexBindingDescriptions;
	m_PipelineVertInStateCreateInfo.vertexAttributeDescriptionCount = m_VertexData.vertexAttributeDescriptionCount;
	m_PipelineVertInStateCreateInfo.pVertexAttributeDescriptions = m_VertexData.pVertexAttributeDescriptions;

	//Input Assembly
	m_PipelineInAssemStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_PipelineInAssemStateCreateInfo.pNext = nullptr;
	m_PipelineInAssemStateCreateInfo.flags = 0;
	m_PipelineInAssemStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; //Style of drawing
	m_PipelineInAssemStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	//Viewport
	m_Viewport.x = 0.0f;
	m_Viewport.y = 0.0f;
	m_Viewport.width = static_cast<float>(m_SwapChainExtent.width);
	m_Viewport.height = static_cast<float>(m_SwapChainExtent.height);
	m_Viewport.minDepth = 0.0f;
	m_Viewport.maxDepth = 1.0f;
	m_Scissor.offset = { 0, 0 };
	m_Scissor.extent = m_SwapChainExtent;

	m_PipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_PipelineViewportStateCreateInfo.viewportCount = 1;
	m_PipelineViewportStateCreateInfo.pViewports = &m_Viewport;
	m_PipelineViewportStateCreateInfo.scissorCount = 1;
	m_PipelineViewportStateCreateInfo.pScissors = &m_Scissor;

	//Rasterizer
	m_PipelineRasterizerStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_PipelineRasterizerStateCreateInfo.depthClampEnable = VK_FALSE; //Forces all geometry into the depth field
	m_PipelineRasterizerStateCreateInfo.rasterizerDiscardEnable = VK_FALSE; //Culls out of bound geometry
	m_PipelineRasterizerStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL; //Fill type
	m_PipelineRasterizerStateCreateInfo.lineWidth = 1.0f;
	m_PipelineRasterizerStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT; 
	m_PipelineRasterizerStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //Specifies the vertex order for faces to be considered front-facing !!!CHECK IBO SO THAT TRIANGLE ARE IN ORDER, ELSE THEY WILL NOT RENDER!!!
	m_PipelineRasterizerStateCreateInfo.depthBiasEnable = VK_FALSE;
	m_PipelineRasterizerStateCreateInfo.depthBiasConstantFactor = 0.0f;
	m_PipelineRasterizerStateCreateInfo.depthBiasClamp = 0.0f;
	m_PipelineRasterizerStateCreateInfo.depthBiasSlopeFactor = 0.0f;

	//Multisampling
	m_PipelineMultisamplingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_PipelineMultisamplingStateCreateInfo.sampleShadingEnable = VK_FALSE; //Currently disabled
	m_PipelineMultisamplingStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	m_PipelineMultisamplingStateCreateInfo.minSampleShading = 1.0f; 
	m_PipelineMultisamplingStateCreateInfo.pSampleMask = nullptr; 
	m_PipelineMultisamplingStateCreateInfo.alphaToCoverageEnable = VK_FALSE; 
	m_PipelineMultisamplingStateCreateInfo.alphaToOneEnable = VK_FALSE;

	//Depth and Stencil testing;
	m_PipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_PipelineDepthStencilStateCreateInfo.pNext = nullptr;
	m_PipelineDepthStencilStateCreateInfo.flags = 0;
	m_PipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	m_PipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	m_PipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS; //Fragment discards
	m_PipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE; //Ranged based testing
	m_PipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE; // Stencil testing
	m_PipelineDepthStencilStateCreateInfo.front = {};
	m_PipelineDepthStencilStateCreateInfo.back = {};
	m_PipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	m_PipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;

	//Colour Blending
	m_PipelineColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	m_PipelineColorBlendAttachment.blendEnable = VK_TRUE; //Alpha blending
	m_PipelineColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; 
	m_PipelineColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	m_PipelineColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; 
	m_PipelineColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; 
	m_PipelineColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	m_PipelineColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	m_PipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_PipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	m_PipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	m_PipelineColorBlendStateCreateInfo.attachmentCount = 1;
	m_PipelineColorBlendStateCreateInfo.pAttachments = &m_PipelineColorBlendAttachment;
	m_PipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	m_PipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	m_PipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	m_PipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	//Dynamic State
	m_PipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	m_PipelineDynamicStateCreateInfo.pNext = nullptr;
	m_PipelineDynamicStateCreateInfo.flags = 0;
	m_PipelineDynamicStateCreateInfo.dynamicStateCount = 2;
	m_PipelineDynamicStateCreateInfo.pDynamicStates = m_DynamicStates;

	//Pipeline Layout
	m_PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	m_PipelineLayoutCreateInfo.pNext = nullptr;
	m_PipelineLayoutCreateInfo.flags = 0;
	m_PipelineLayoutCreateInfo.setLayoutCount = 1; //For the UBO
	m_PipelineLayoutCreateInfo.pSetLayouts = &m_DescSetLayout;
	m_PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	m_PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	VkResult result = vkCreatePipelineLayout(m_Device, &m_PipelineLayoutCreateInfo, nullptr, &m_PipelineLayout);
	VkAssert(result);

	m_GraphicPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	m_GraphicPipelineCreateInfo.pNext = nullptr;
	m_GraphicPipelineCreateInfo.flags = 0;
	m_GraphicPipelineCreateInfo.stageCount = 2;
	m_GraphicPipelineCreateInfo.pStages = shaderStages;
	m_GraphicPipelineCreateInfo.pVertexInputState = &m_PipelineVertInStateCreateInfo;
	m_GraphicPipelineCreateInfo.pInputAssemblyState = &m_PipelineInAssemStateCreateInfo;
	m_GraphicPipelineCreateInfo.pViewportState = &m_PipelineViewportStateCreateInfo;
	m_GraphicPipelineCreateInfo.pRasterizationState = &m_PipelineRasterizerStateCreateInfo;
	m_GraphicPipelineCreateInfo.pMultisampleState = &m_PipelineMultisamplingStateCreateInfo;
	m_GraphicPipelineCreateInfo.pDepthStencilState = &m_PipelineDepthStencilStateCreateInfo;
	m_GraphicPipelineCreateInfo.pColorBlendState = &m_PipelineColorBlendStateCreateInfo;
	m_GraphicPipelineCreateInfo.pDynamicState = nullptr;
	m_GraphicPipelineCreateInfo.layout = m_PipelineLayout;
	m_GraphicPipelineCreateInfo.renderPass = m_RenderPass;
	m_GraphicPipelineCreateInfo.subpass = 0;
	m_GraphicPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	m_GraphicPipelineCreateInfo.basePipelineIndex = -1;

	result = vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &m_GraphicPipelineCreateInfo, nullptr, &m_Pipeline);
	VkAssert(result);
}

void Vulkan::DestroyGraphicPipeline()
{
	vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
	vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
}

void Vulkan::CreateFramebuffers()
{
	m_SwapChainFramebuffer.resize(m_SwapChainImageViews.size());
	m_FramebufferCreateInfos.reserve(m_SwapChainImageViews.size());
	for (int i = 0; i < static_cast<int32_t>(m_SwapChainImageViews.size()); i++)
	{
		std::vector<VkImageView> attachments = { m_SwapChainImageViews[i], m_DepthImageView };
		VkFramebufferCreateInfo temp;
		temp.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		temp.pNext = nullptr;
		temp.flags = 0;
		temp.renderPass = m_RenderPass;
		temp.attachmentCount = static_cast<uint32_t>(attachments.size());
		temp.pAttachments = attachments.data();
		temp.width = m_SwapChainExtent.width;
		temp.height = m_SwapChainExtent.height;
		temp.layers = 1;
		m_FramebufferCreateInfos.push_back(temp);

		VkResult result = vkCreateFramebuffer(m_Device, &temp, nullptr, &m_SwapChainFramebuffer[i]);
		VkAssert(result);
	}
}

void Vulkan::DestroyFramebuffers()
{
	for (auto framebuffer : m_SwapChainFramebuffer)
	{
		vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
	}
	m_FramebufferCreateInfos.clear();
}

void Vulkan::CreateCommandPool()
{
	
	m_CmdPools.resize(m_QueueFamilyCount);
	m_CmdPoolsCreateInfo.reserve(m_QueueFamilyCount);
	for (int i = 0; i < static_cast<int32_t>(m_QueueFamilyCount); i++)
	{
		VkCommandPoolCreateInfo temp = {};
		temp.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		temp.pNext = nullptr;
		temp.queueFamilyIndex = m_DeviceQueueCreateInfo[i].queueFamilyIndex;
		temp.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		m_CmdPoolsCreateInfo.push_back(temp);

		VkResult result = vkCreateCommandPool(m_Device, &temp, nullptr, &m_CmdPools[i]);
		VkAssert(result);
	}
}

void Vulkan::DestroyCommandPool()
{
	for (auto cmdPool : m_CmdPools)
	{
		vkDestroyCommandPool(m_Device, cmdPool, nullptr);
	}
}

void Vulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	std::vector<uint32_t>queueFamilyIndices; //TODO: Look into this further
	queueFamilyIndices.reserve(m_QueueFamilyProp.size());
	for (int i = 0; i < static_cast<int32_t>(m_QueueFamilyProp.size()); i++)
	{
		queueFamilyIndices.push_back(i);
	}

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	bufferCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(m_QueueFamilyProp.size());
	bufferCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
	m_BufferCreateInfos.push_back(bufferCreateInfo);

	VkResult result = vkCreateBuffer(m_Device, &bufferCreateInfo, nullptr, &buffer);
	VkAssert(result);
	
	m_MemRequirements.resize(m_MemRequirements.size() + 1);
	vkGetBufferMemoryRequirements(m_Device, buffer, &m_MemRequirements.back());

	std::vector<bool> foundSuitableMemType(m_DeviceMemProp.memoryTypeCount, false);
	bool foundGeneral = false;
	for (uint32_t i = 0; i < m_DeviceMemProp.memoryTypeCount; i++)
	{
		if ((m_MemRequirements.back().memoryTypeBits & (1 << i))
			&& (m_DeviceMemProp.memoryTypes[i].propertyFlags & properties)
			== properties)
		{
			foundSuitableMemType[i] = true;
			foundGeneral = true;
		}
	}
	if (!foundGeneral)
		std::cout << "Vulkan could not find a suitable memory for the allocation of the Vertex buffer!" << std::endl;

	uint32_t memoryTypeIndex;
	for (auto index : foundSuitableMemType)
	{
		if (index == true) //Pick the first valid;
		{
			memoryTypeIndex = foundSuitableMemType.at(index);
			break;
		}
	}

	VkMemoryAllocateInfo memAllocInfo;
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.pNext = nullptr;
	memAllocInfo.allocationSize = m_MemRequirements.back().size;
	memAllocInfo.memoryTypeIndex = memoryTypeIndex;
	m_MemAllocInfos.push_back(memAllocInfo);

	result = vkAllocateMemory(m_Device, &memAllocInfo, nullptr, &bufferMemory);
	VkAssert(result);

	result = vkBindBufferMemory(m_Device, buffer, bufferMemory, 0);
	VkAssert(result);
}

void Vulkan::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	uint32_t cmdPoolIndex = 0;
	for (auto& queueGeneralSupport : m_QueueGeneralPresentSupport)
	{
		if (queueGeneralSupport.queueTransferBit == true
			&& queueGeneralSupport.queueGraphicsBit == false
			&& queueGeneralSupport.queueComputeBit == false)
		{
			cmdPoolIndex = queueGeneralSupport.queueFamily;
		}
	}

	VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferAllocInfo.commandPool = m_CmdPools[cmdPoolIndex];
	cmdBufferAllocInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuffer;
	VkResult result = vkAllocateCommandBuffers(m_Device, &cmdBufferAllocInfo, &cmdBuffer);
	VkAssert(result);

	VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	result = vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
	VkAssert(result);

	VkBufferCopy bufferCopy;
	bufferCopy.srcOffset = 0;
	bufferCopy.dstOffset = 0;
	bufferCopy.size = size;
	
	vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);

	vkEndCommandBuffer(cmdBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;

	vkQueueSubmit(m_Queues[cmdPoolIndex], 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_Queues[cmdPoolIndex]);

	vkFreeCommandBuffers(m_Device, m_CmdPools[cmdPoolIndex], 1, &cmdBuffer);
}

void Vulkan::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo temp = {};
	temp.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	temp.pNext = nullptr;
	temp.flags = 0;
	temp.imageType = VK_IMAGE_TYPE_2D;
	temp.extent.width = width;
	temp.extent.height = height;
	temp.extent.depth = 1;
	temp.mipLevels = 1;
	temp.arrayLayers = 1;
	temp.format = format;
	temp.tiling = tiling;
	temp.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	temp.usage = usage;
	temp.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	temp.samples = VK_SAMPLE_COUNT_1_BIT;
	m_ImageCreateInfos.push_back(temp);

	VkResult result = vkCreateImage(m_Device, &m_ImageCreateInfos.back(), nullptr, &image);
	VkAssert(result);

	m_MemRequirements.resize(m_MemRequirements.size() + 1);
	vkGetImageMemoryRequirements(m_Device, image, &m_MemRequirements.back());

	std::vector<bool> foundSuitableMemType(m_DeviceMemProp.memoryTypeCount, false);
	bool foundGeneral = false;
	for (uint32_t i = 0; i < m_DeviceMemProp.memoryTypeCount; i++)
	{
		if ((m_MemRequirements.back().memoryTypeBits & (1 << i))
			&& (m_DeviceMemProp.memoryTypes[i].propertyFlags & properties)
			== properties)
		{
			foundSuitableMemType[i] = true;
			foundGeneral = true;
		}
	}
	if (!foundGeneral)
		std::cout << "Vulkan could not find a suitable memory for the allocation of the Image buffer!" << std::endl;

	uint32_t memoryTypeIndex;
	for (auto index : foundSuitableMemType)
	{
		if (index == true) //Pick the first valid;
		{
			memoryTypeIndex = foundSuitableMemType.at(index);
			break;
		}
	}

	VkMemoryAllocateInfo memAllocInfo;
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.pNext = nullptr;
	memAllocInfo.allocationSize = m_MemRequirements.back().size;
	memAllocInfo.memoryTypeIndex = memoryTypeIndex;
	m_MemAllocInfos.push_back(memAllocInfo);

	result = vkAllocateMemory(m_Device, &memAllocInfo, nullptr, &imageMemory);
	VkAssert(result);

	result = vkBindImageMemory(m_Device, image, imageMemory, 0);
	VkAssert(result);
}

VkCommandBuffer Vulkan::BeginSingleTimeCommand()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_CmdPools[0]; //Default use first;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuffer;
	vkAllocateCommandBuffers(m_Device, &allocInfo, &cmdBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdBuffer, &beginInfo);

	return cmdBuffer;
}

void Vulkan::EndSingleTimeCommand(VkCommandBuffer cmdBuffer) {
	vkEndCommandBuffer(cmdBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;

	vkQueueSubmit(m_Queues[0], 1, &submitInfo, VK_NULL_HANDLE); //Default use first;
	vkQueueWaitIdle(m_Queues[0]);

	vkFreeCommandBuffers(m_Device, m_CmdPools[0], 1, &cmdBuffer);
}

void Vulkan::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer cmdBuffer = BeginSingleTimeCommand();
	
	VkBufferImageCopy imageCopy = {};
	imageCopy.bufferOffset = 0;
	imageCopy.bufferRowLength = 0;
	imageCopy.bufferImageHeight = 0;
	imageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.imageSubresource.mipLevel = 0;
	imageCopy.imageSubresource.baseArrayLayer = 0;
	imageCopy.imageSubresource.layerCount = 1;
	imageCopy.imageOffset = { 0, 0, 0 };
	imageCopy.imageExtent = {width, height,	1};

	vkCmdCopyBufferToImage(cmdBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
	
	EndSingleTimeCommand(cmdBuffer);
}

void Vulkan::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer cmdBuffer = BeginSingleTimeCommand();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (HasStencilComponent(format))
		{
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && 
		newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
	{
		std::cout << "Vulkan can't use the described layout transition" << std::endl;
	}

	vkCmdPipelineBarrier(
		cmdBuffer,
		sourceStage,
		destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	EndSingleTimeCommand(cmdBuffer);
}

VkFormat Vulkan::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(m_PhysicalDevices[0], format, &formatProps);

		if (tiling == VK_IMAGE_TILING_LINEAR && (formatProps.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (formatProps.optimalTilingFeatures & features) == features)
		{
			return format;
		}
		else
		{
			std::cout << "Vulkan failed to find a supported format!" << std::endl;
			return format;
		}
	}
	return VkFormat{ VK_FORMAT_UNDEFINED };
}

VkFormat Vulkan::FindDepthFormat()
{
	return FindSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool Vulkan::HasStencilComponent(VkFormat format) 
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Vulkan::CreateDepthImage()
{
	VkFormat depthFormat = FindDepthFormat();

	CreateImage(m_SwapChainExtent.width, m_SwapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMem);
	m_DepthImageView = CreateImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void Vulkan::DestroyDepthImage()
{
	vkDestroyImageView(m_Device, m_DepthImageView, nullptr);
	vkDestroyImage(m_Device, m_DepthImage, nullptr);
	vkFreeMemory(m_Device, m_DepthImageMem, nullptr);
}

void Vulkan::CreateTextureImage()
{
	int width, height, channels;
	unsigned char* pixelData = stbi_load(m_FilepathTex.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	m_TexSize = width * height * STBI_rgb_alpha;

	CreateBuffer(m_TexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingTexBuffer, m_StagingTexBufferMem);
	
	void* texData;
	VkResult result = vkMapMemory(m_Device, m_StagingTexBufferMem, 0, m_TexSize, 0, &texData);
	VkAssert(result);
	memcpy(texData, pixelData, static_cast<size_t>(m_TexSize));
	vkUnmapMemory(m_Device, m_StagingTexBufferMem);

	stbi_image_free(pixelData);

	CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TexImage, m_TexBufferMem);

	TransitionImageLayout(m_TexImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(m_StagingTexBuffer, m_TexImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	TransitionImageLayout(m_TexImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Vulkan::DestroyTextureImage()
{
	vkDestroyImage(m_Device, m_TexImage, nullptr);
	vkFreeMemory(m_Device, m_TexBufferMem, nullptr);
}

VkImageView Vulkan::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo temp = {};
	temp.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	temp.image = image;
	temp.viewType = VK_IMAGE_VIEW_TYPE_2D;
	temp.format = format;
	temp.subresourceRange.aspectMask = aspectFlags;
	temp.subresourceRange.baseMipLevel = 0;
	temp.subresourceRange.levelCount = 1;
	temp.subresourceRange.baseArrayLayer = 0;
	temp.subresourceRange.layerCount = 1;
	m_TexImageViewCreateInfos.push_back(temp);

	VkImageView imageView;
	VkResult result = vkCreateImageView(m_Device, &temp, nullptr, &imageView);
	VkAssert(result);

	return imageView;
}

void Vulkan::CreateTextureImageView()
{
	m_TexImageView = CreateImageView(m_TexImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

void Vulkan::DestroyTextureImageView()
{
	vkDestroyImageView(m_Device, m_TexImageView, nullptr);
}

void Vulkan::CreateTextureSampler()
{
	m_SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	m_SamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	m_SamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	m_SamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	m_SamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	m_SamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	if (m_DeviceFeatures.samplerAnisotropy == 1)
	{
		m_SamplerCreateInfo.anisotropyEnable = VK_TRUE;
		m_SamplerCreateInfo.maxAnisotropy = 16;
	}
	else
	{
		m_SamplerCreateInfo.anisotropyEnable = VK_FALSE;
		m_SamplerCreateInfo.maxAnisotropy = 1;
	}
	m_SamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	m_SamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	m_SamplerCreateInfo.compareEnable = VK_FALSE;
	m_SamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	m_SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	m_SamplerCreateInfo.mipLodBias = 0.0f;
	m_SamplerCreateInfo.minLod = 0.0f;
	m_SamplerCreateInfo.maxLod = 0.0f;

	VkResult result = vkCreateSampler(m_Device, &m_SamplerCreateInfo, nullptr, &m_Sampler);
	VkAssert(result);
}

void Vulkan::DestroyTextureSampler()
{
	vkDestroySampler(m_Device, m_Sampler, nullptr);
}

void Vulkan::CreateVertexBuffer()
{
	VkDeviceSize bufferSize = m_VertexData.verticesSize;
	//Vertexbuffer CPU Side
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingVertexBuffer, m_StagingVertexBufferMem);

	void* vertexData;
	VkResult result = vkMapMemory(m_Device, m_StagingVertexBufferMem, 0, bufferSize, 0, &vertexData);
	VkAssert(result);
	memcpy(vertexData, m_VertexData.pVertexData, static_cast<size_t>(bufferSize));
	vkUnmapMemory(m_Device, m_StagingVertexBufferMem);

	//VertexBuffer GPU Side
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMem);

	CopyBuffer(m_StagingVertexBuffer, m_VertexBuffer, bufferSize);
	vkFreeMemory(m_Device, m_StagingVertexBufferMem, nullptr);
	vkDestroyBuffer(m_Device, m_StagingVertexBuffer, nullptr);
}

void Vulkan::DestroyVertexBuffer()
{
	vkFreeMemory(m_Device, m_VertexBufferMem, nullptr);
	vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
}

void Vulkan::CreateIndexBuffer()
{
	VkDeviceSize bufferSize = m_IndexData.verticesSize;
	//Indexbuffer CPU Side
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingIndexBuffer, m_StagingIndexBufferMem);

	void* indexData;
	VkResult result = vkMapMemory(m_Device, m_StagingIndexBufferMem, 0, bufferSize, 0, &indexData);
	VkAssert(result);
	memcpy(indexData, m_IndexData.pIndexData, static_cast<size_t>(bufferSize));
	vkUnmapMemory(m_Device, m_StagingIndexBufferMem);

	//IndexBuffer GPU Side
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMem);

	CopyBuffer(m_StagingIndexBuffer, m_IndexBuffer, bufferSize);
	vkFreeMemory(m_Device, m_StagingIndexBufferMem, nullptr);
	vkDestroyBuffer(m_Device, m_StagingIndexBuffer, nullptr);
}

void Vulkan::DestroyIndexBuffer()
{
	vkFreeMemory(m_Device, m_IndexBufferMem, nullptr);
	vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
}

void Vulkan::CreateUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	m_UniformBuffers.resize(m_SwapChainImages.size());
	m_UniformBuffersMem.resize(m_SwapChainImages.size());

	for (int i = 0; i < static_cast<int32_t>(m_SwapChainImages.size()); i++)
	{
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i], m_UniformBuffersMem[i]);
	}
}

void Vulkan::DestroyUniformBuffer()
{
	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		vkDestroyBuffer(m_Device, m_UniformBuffers[i], nullptr);
		vkFreeMemory(m_Device, m_UniformBuffersMem[i], nullptr);
	}
}

void Vulkan::CreateDiscriptorPool()
{
	m_PoolSizes.resize(m_DescSetLayoutBinds.size());
	m_PoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	m_PoolSizes[0].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());
	m_PoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	m_PoolSizes[1].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());

	m_PoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	m_PoolCreateInfo.poolSizeCount = static_cast<uint32_t>(m_PoolSizes.size());
	m_PoolCreateInfo.pPoolSizes = m_PoolSizes.data();
	m_PoolCreateInfo.maxSets = static_cast<uint32_t>(m_SwapChainImages.size());

	VkResult result = vkCreateDescriptorPool(m_Device, &m_PoolCreateInfo, nullptr, &m_Pool);
	VkAssert(result);
}

void Vulkan::DestroyDescriptionorPool()
{
	vkDestroyDescriptorPool(m_Device, m_Pool, nullptr);
}

void Vulkan::UpdateUniformBuffers()
{
	void* uniformData;
	VkResult result = vkMapMemory(m_Device, m_UniformBuffersMem[m_UBO.currentImage], 0, sizeof(UniformBufferObject), 0, &uniformData);
	VkAssert(result);
	memcpy(uniformData, &m_UBO, static_cast<size_t>(sizeof(UniformBufferObject)));
	vkUnmapMemory(m_Device, m_UniformBuffersMem[m_UBO.currentImage]);
}

void Vulkan::CreateDescriptorSets()
{
	m_DescSetLayouts.reserve(m_SwapChainImages.size());
	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		m_DescSetLayouts.push_back(m_DescSetLayout);
	}
		
	m_DescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	m_DescSetAllocInfo.descriptorPool = m_Pool;
	m_DescSetAllocInfo.descriptorSetCount = static_cast<uint32_t>(m_SwapChainImages.size());
	m_DescSetAllocInfo.pSetLayouts = m_DescSetLayouts.data();

	m_DescSets.resize(m_SwapChainImages.size());
	VkResult result = vkAllocateDescriptorSets(m_Device, &m_DescSetAllocInfo, m_DescSets.data());
	VkAssert(result);

	m_DescBufferInfos.reserve(m_SwapChainImages.size());
	m_DescImageInfos.reserve(m_SwapChainImages.size());
	m_WriteDescSets.reserve(m_SwapChainImages.size());
	for (size_t i = 0; i < m_SwapChainImages.size(); i++) 
	{
		VkDescriptorBufferInfo tempDescBufferInfo = {};
		tempDescBufferInfo.buffer = m_UniformBuffers[i];
		tempDescBufferInfo.offset = 0;
		tempDescBufferInfo.range = sizeof(UniformBufferObject);
		m_DescBufferInfos.push_back(tempDescBufferInfo);

		VkDescriptorImageInfo tempDescImageInfo = {};
		tempDescImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		tempDescImageInfo.imageView = m_TexImageView;
		tempDescImageInfo.sampler = m_Sampler;

		VkWriteDescriptorSet tempWriteDescSet = {};
		tempWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		tempWriteDescSet.dstSet = m_DescSets[i];
		tempWriteDescSet.dstBinding = 0; //Index in Vertex shader
		tempWriteDescSet.dstArrayElement = 0; //Nou using an array
		tempWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		tempWriteDescSet.descriptorCount = 1;
		tempWriteDescSet.pBufferInfo = &tempDescBufferInfo;
		tempWriteDescSet.pImageInfo = nullptr;
		tempWriteDescSet.pTexelBufferView = nullptr;
		m_WriteDescSets.push_back(tempWriteDescSet);

		tempWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		tempWriteDescSet.dstSet = m_DescSets[i];
		tempWriteDescSet.dstBinding = 1; //Index in Fragment shader
		tempWriteDescSet.dstArrayElement = 0; //Nou using an array
		tempWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		tempWriteDescSet.descriptorCount = 1;
		tempWriteDescSet.pBufferInfo = nullptr;
		tempWriteDescSet.pImageInfo = &tempDescImageInfo;
		tempWriteDescSet.pTexelBufferView = nullptr;
		m_WriteDescSets.push_back(tempWriteDescSet);

		vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(m_WriteDescSets.size()), m_WriteDescSets.data(), 0, nullptr);
	}
}

void Vulkan::DestroyDescriptorSets()
{
}

void Vulkan::CreateCommandbuffers()
{
	
	m_CmdBuffers.resize(m_SwapChainFramebuffer.size());
	m_CmdBufferAllocInfo.reserve(m_SwapChainFramebuffer.size());
	for (int i = 0; i < static_cast<int32_t>(m_SwapChainFramebuffer.size()); i++)
	{
		VkCommandBufferAllocateInfo temp = {};
		temp.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		temp.commandBufferCount = 1;
		temp.commandPool = m_CmdPools[0]; //Uses the CmdPools from PhysicalDevice[0] and QueueFamily[0]
		temp.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		m_CmdBufferAllocInfo.push_back(temp);

		VkResult result = vkAllocateCommandBuffers(m_Device, &temp, &m_CmdBuffers[i]);
		VkAssert(result);
	}
}

void Vulkan::DestroyCommandbuffers()
{
	vkFreeCommandBuffers(m_Device, m_CmdPools[0], static_cast<uint32_t>(m_CmdBuffers.size()), m_CmdBuffers.data()); //Uses the CmdPools from PhysicalDevice[0] and QueueFamily[0]
	m_CmdBufferAllocInfo.clear();
}

void Vulkan::CreateSemaphoresAndFences()
{
	m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	m_SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	m_SemaphoreCreateInfo.pNext = nullptr;
	m_SemaphoreCreateInfo.flags = 0;

	m_FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	m_FenceCreateInfo.pNext = nullptr;
	m_FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkResult result;
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		result = vkCreateSemaphore(m_Device, &m_SemaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphores[i]);
		VkAssert(result);
		result = vkCreateSemaphore(m_Device, &m_SemaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]);
		VkAssert(result);
		result = vkCreateFence(m_Device, &m_FenceCreateInfo, nullptr, &m_InFlightFences[i]);
		VkAssert(result);
	}
}

void Vulkan::DestroySemaphoresAndFences()
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
	}
}

void Vulkan::BeignCommandbufferRecord() //Call when drawing!
{
	m_CmdBufferBeginInfos.reserve(m_CmdBuffers.size());
	for (int i = 0; i < static_cast<int32_t>(m_CmdBuffers.size()); i++)
	{
		VkCommandBufferBeginInfo temp = {};
		temp.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		temp.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		temp.pInheritanceInfo = nullptr;
		m_CmdBufferBeginInfos.push_back(temp);

		VkResult result = vkBeginCommandBuffer(m_CmdBuffers[i], &temp);
		VkAssert(result);
	}
}

void Vulkan::EndCommandbufferRecord()//Call when drawing!
{
	for (auto cmdBuffer : m_CmdBuffers)
	{
		vkEndCommandBuffer(cmdBuffer);
	}
}

void Vulkan::BeginRenderPass() //Call when drawing!
{
	m_ClearColours[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_ClearColours[1].depthStencil = { 1.0f, 0 };
	for (int i = 0; i < static_cast<int32_t>(m_CmdBuffers.size()); i++)
	{
		VkRenderPassBeginInfo temp = {};
		temp.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		temp.pNext = nullptr;
		temp.renderPass = m_RenderPass;
		temp.framebuffer = m_SwapChainFramebuffer[i];
		temp.renderArea.offset = { 0, 0 };
		temp.renderArea.extent = m_SwapChainExtent;
		temp.clearValueCount = static_cast<uint32_t>(m_ClearColours.size());
		temp.pClearValues = m_ClearColours.data();
		m_RenderPassBeginInfos.push_back(temp);

		vkCmdBeginRenderPass(m_CmdBuffers[i], &temp, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_CmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
		vkCmdBindVertexBuffers(m_CmdBuffers[i], 0, 1, &m_VertexBuffer, m_OffsetVert);
		vkCmdBindIndexBuffer(m_CmdBuffers[i], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(m_CmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescSets[i], 0, nullptr);
		vkCmdDrawIndexed(m_CmdBuffers[i], m_IndexData.verticesCount, 1, 0, 0, 0);
	}
}

void Vulkan::EndRenderPass() //Call when drawing!
{
	for (auto cmdBuffer : m_CmdBuffers)
	{
		vkCmdEndRenderPass(cmdBuffer);
	}
}

void Vulkan::Update() //Call inside a loop!
{
	glfwPollEvents();
}

//private:
void Vulkan::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Vulkan*>(glfwGetWindowUserPointer(window));
	app->m_FramebufferResized = true;
}

VkExtent2D Vulkan::ChooseNewSwapChainExtent()
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

std::ostream& operator<< (std::ostream& stream, const VkLayerProperties& properties)
{
	stream << "Name: " << properties.layerName << std::endl;
	stream << "Specification Version: " << properties.specVersion << std::endl;
	stream << "Implementation Version: " << properties.implementationVersion << std::endl;
	stream << "Description: " << properties.description << std::endl;
	return stream;
}
void Vulkan::PrintAllVkLayerProperties(const VkLayerProperties* properties, size_t count)
{
	std::cout << "Number of Layers: " << count << std::endl;
	for (int i = 0; i < (int)count; i++)
	{
		std::cout << "Layer " << i + 1 << std::endl;
		std::cout << properties[i] << std::endl;
	}
}

std::ostream& operator<< (std::ostream& stream, const VkExtensionProperties& properties)
{
	stream << "Name: " << properties.extensionName << std::endl;
	stream << "Specification Version: " << properties.specVersion << std::endl;
	return stream;
}
void Vulkan::PrintAllVkExtensionProperties(const VkExtensionProperties* properties, size_t count)
{
	std::cout << "Number of Extensions: " << count << std::endl;
	for (int i = 0; i < (int)count; i++)
	{
		std::cout << "Extension " << i + 1 << std::endl;
		std::cout << properties[i] << std::endl;
	}
}

std::ostream& operator<< (std::ostream& stream, const VkPhysicalDeviceProperties& properties)
{

	stream << "Name: " << properties.deviceName << std::endl;
	stream << "API Version: Vulkan " << VK_VERSION_MAJOR(properties.apiVersion) << "." << VK_VERSION_MINOR(properties.apiVersion) << "." << VK_VERSION_PATCH(properties.apiVersion) << std::endl;
	stream << "Device Version: " << properties.driverVersion << std::endl;
	stream << "VendorID: " << properties.vendorID << std::endl;
	stream << "DeviceID: " << properties.deviceID << std::endl;
	stream << "DeviceType: " << properties.deviceType << std::endl;
	//stream << properties.pipelineCacheUUID << std::endl;
	stream << "LimitsDiscreteQueueProperties: " << properties.limits.discreteQueuePriorities << std::endl;
	//stream << properties.sparseProperties << std::endl;
	return stream;
}
void Vulkan::PrintVkPhysicalDeviceProperties(const VkPhysicalDeviceProperties& properties)
{
	std::cout << properties << std::endl;
}

std::ostream& operator<< (std::ostream& stream, const VkPhysicalDeviceFeatures& properties)
{
	stream << "robustBufferAccess: " << properties.robustBufferAccess << std::endl;
	stream << "fullDrawIndexUint32: " << properties.fullDrawIndexUint32 << std::endl;
	stream << "imageCubeArray: " << properties.imageCubeArray << std::endl;
	stream << "independentBlend: " << properties.independentBlend << std::endl;
	stream << "geometryShader: " << properties.geometryShader << std::endl;
	stream << "tessellationShader: " << properties.tessellationShader << std::endl;
	stream << "sampleRateShading: " << properties.sampleRateShading << std::endl;
	stream << "dualSrcBlend: " << properties.dualSrcBlend << std::endl;
	stream << "logicOp: " << properties.logicOp << std::endl;
	stream << "multiDrawIndirect: " << properties.multiDrawIndirect << std::endl;
	stream << "drawIndirectFirstInstance: " << properties.drawIndirectFirstInstance << std::endl;
	stream << "depthClamp: " << properties.depthClamp << std::endl;
	stream << "depthBiasClamp: " << properties.depthBiasClamp << std::endl;
	stream << "fillModeNonSolid: " << properties.fillModeNonSolid << std::endl;
	stream << "depthBounds: " << properties.depthBounds << std::endl;
	stream << "wideLines: " << properties.wideLines << std::endl;
	stream << "largePoints: " << properties.largePoints << std::endl;
	stream << "alphaToOne: " << properties.alphaToOne << std::endl;
	stream << "multiViewport: " << properties.multiViewport << std::endl;
	stream << "samplerAnisotropy: " << properties.samplerAnisotropy << std::endl;
	stream << "textureCompressionETC2: " << properties.textureCompressionETC2 << std::endl;
	stream << "textureCompressionASTC_LDR: " << properties.textureCompressionASTC_LDR << std::endl;
	stream << "textureCompressionBC: " << properties.textureCompressionBC << std::endl;
	stream << "occlusionQueryPrecise: " << properties.occlusionQueryPrecise << std::endl;
	stream << "pipelineStatisticsQuery: " << properties.pipelineStatisticsQuery << std::endl;
	stream << "vertexPipelineStoresAndAtomics: " << properties.vertexPipelineStoresAndAtomics << std::endl;
	stream << "fragmentStoresAndAtomics: " << properties.fragmentStoresAndAtomics << std::endl;
	stream << "shaderTessellationAndGeometryPointSize: " << properties.shaderTessellationAndGeometryPointSize << std::endl;
	stream << "shaderImageGatherExtended: " << properties.shaderImageGatherExtended << std::endl;
	stream << "shaderStorageImageExtendedFormats: " << properties.shaderStorageImageExtendedFormats << std::endl;
	stream << "shaderStorageImageMultisample: " << properties.shaderStorageImageMultisample << std::endl;
	stream << "shaderStorageImageReadWithoutFormat: " << properties.shaderStorageImageReadWithoutFormat << std::endl;
	stream << "shaderStorageImageWriteWithoutFormat: " << properties.shaderStorageImageWriteWithoutFormat << std::endl;
	stream << "shaderUniformBufferArrayDynamicIndexing: " << properties.shaderUniformBufferArrayDynamicIndexing << std::endl;
	stream << "shaderSampledImageArrayDynamicIndexing: " << properties.shaderSampledImageArrayDynamicIndexing << std::endl;
	stream << "shaderStorageBufferArrayDynamicIndexing: " << properties.shaderStorageBufferArrayDynamicIndexing << std::endl;
	stream << "shaderStorageImageArrayDynamicIndexing: " << properties.shaderStorageImageArrayDynamicIndexing << std::endl;
	stream << "shaderClipDistance: " << properties.shaderClipDistance << std::endl;
	stream << "shaderCullDistance: " << properties.shaderCullDistance << std::endl;
	stream << "shaderFloat64: " << properties.shaderFloat64 << std::endl;
	stream << "shaderInt64: " << properties.shaderInt64 << std::endl;
	stream << "shaderInt16: " << properties.shaderInt16 << std::endl;
	stream << "shaderResourceResidency: " << properties.shaderResourceResidency << std::endl;
	stream << "shaderResourceMinLod: " << properties.shaderResourceMinLod << std::endl;
	stream << "sparseBinding: " << properties.sparseBinding << std::endl;
	stream << "sparseResidencyBuffer: " << properties.sparseResidencyBuffer << std::endl;
	stream << "sparseResidencyImage2D: " << properties.sparseResidencyImage2D << std::endl;
	stream << "sparseResidencyImage3D: " << properties.sparseResidencyImage3D << std::endl;
	stream << "sparseResidency2Samples: " << properties.sparseResidency2Samples << std::endl;
	stream << "sparseResidency4Samples: " << properties.sparseResidency4Samples << std::endl;
	stream << "sparseResidency8Samples: " << properties.sparseResidency8Samples << std::endl;
	stream << "sparseResidency16Samples: " << properties.sparseResidency16Samples << std::endl;
	stream << "sparseResidencyAliased: " << properties.sparseResidencyAliased << std::endl;
	stream << "variableMultisampleRate: " << properties.variableMultisampleRate << std::endl;
	stream << "inheritedQueries: " << properties.inheritedQueries << std::endl;

	return stream;
}
void Vulkan::PrintVkPhysicalDeviceFeatures(const VkPhysicalDeviceFeatures& features)
{
	std::cout << features << std::endl;
}

std::ostream& operator<< (std::ostream& stream, const VkPhysicalDeviceMemoryProperties& properties)
{
	stream << "Memory Type Count: " << properties.memoryTypeCount << std::endl;
	for(int i = 0; i < static_cast<int32_t>(properties.memoryTypeCount); i++)
		stream << i << " Memory Flag: " << properties.memoryTypes[i].propertyFlags << ", Heap Index: " << properties.memoryTypes[i].heapIndex << std::endl;
	
	stream << "Memory Heap Count: " << properties.memoryHeapCount << std::endl;
	for(int i = 0; i < static_cast<int32_t>(properties.memoryHeapCount); i++)
		stream << i << " Heap Size: " << properties.memoryHeaps[i].size << "B, Heap Flag: " << properties.memoryHeaps[i].flags << std::endl;
	
	return stream;
}
void Vulkan::PrintVkPhysicalDeviceMemoryProperties(const VkPhysicalDeviceMemoryProperties& properties)
{
	std::cout << properties << std::endl;
}

std::ostream& operator<< (std::ostream& stream, const VkQueueFamilyProperties& properties)
{
	stream << "Queue Flag: VK_QUEUE_GRAPHICS_BIT: " << ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
	stream << "Queue Flag: VK_QUEUE_COMPUTE_BIT : " << ((properties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
	stream << "Queue Flag: VK_QUEUE_TRANSFER_BIT: " << ((properties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
	stream << "Queue Flag: VK_QUEUE_SPARSE_BINDING_BIT: " << ((properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
	stream << "Number of Queue: " << properties.queueCount << std::endl;
	stream << "TimeStampValidBit: " << properties.timestampValidBits << std::endl;
	stream << "MinImageTransferGranularityWidth: " << properties.minImageTransferGranularity.width << std::endl;
	stream << "MinImageTransferGranularityHeight: " << properties.minImageTransferGranularity.height << std::endl;
	stream << "MinImageTransferGranularityDepth: " << properties.minImageTransferGranularity.depth << std::endl;
	return stream;
}
void Vulkan::PrintAllVkQueueFamilyProperties(const VkQueueFamilyProperties* properties, size_t count)
{
	std::cout << "Number of Queue Families: " << count << std::endl;
	for (int i = 0; i < (int)count; i++)
	{
		std::cout << "Queue Family " << i + 1 << std::endl;
		std::cout << properties[i] << std::endl;
	}
}
