#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"

class Vulkan
{
private:
#define ASSERT(x) if(!(x)) __debugbreak();
#define VkAssert(x) ASSERT(VkLogCall(__FILE__, __LINE__, x));

	bool VkLogCall(const char* file, int line, VkResult x)
	{
		if (x != VK_SUCCESS)
		{
			std::cout << "[Vulkan Error] (" << x << "): " << ", " << file << ":" << line << std::endl;
			return false;
		}
		return true;
	}

#define GLFWAssert(x) ASSERT(GLFWLogCall(__FILE__, __LINE__, x));

	bool GLFWLogCall(const char* file, int line, int x)
	{
		if (x != GLFW_TRUE)
		{
			std::cout << "[GLFW Error] (" << x << "): " << ", " << file << ":" << line << std::endl;
			return false;
		}
		return true;
	}
		
public:
//private:

	//CreateInstance()
	VkInstance m_Instance;
	VkApplicationInfo m_AppInfo;
	VkInstanceCreateInfo m_InstanceInfo;
	
	uint32_t m_Inst_LayerCount = 0; //Layer and Extensions
	uint32_t m_Inst_ExtCount = 0;
	std::vector<VkLayerProperties> m_Inst_LayerProp;
	std::vector<VkExtensionProperties> m_Inst_ExtProp;
	std::vector<const char*>m_Inst_LayersStrings = { "VK_LAYER_LUNARG_standard_validation" };
	std::vector<const char*>m_Inst_ExtStrings = { }; //GLFW extenstion are added.

	//CreateDevice()
	VkDevice m_Device;
	uint32_t m_DeviceCount;
	std::vector<VkPhysicalDevice> m_PhysicalDevices;
	VkPhysicalDeviceProperties m_DeviceProperties; //Only for m_PhysicalDevices[0]
	VkPhysicalDeviceFeatures m_DeviceFeatures;
	VkPhysicalDeviceMemoryProperties m_DeviceMemProp;

	uint32_t m_Device_LayerCount = 0; //Layer and Extensions
	uint32_t m_Device_ExtCount = 0;
	std::vector<VkLayerProperties> m_Device_LayerProp;
	std::vector<VkExtensionProperties> m_Device_ExtProp;
	std::vector<const char*>m_Device_LayersStrings = { "VK_LAYER_LUNARG_standard_validation" };
	std::vector<const char*>m_Device_ExtStrings = { "VK_KHR_swapchain" };

	uint32_t m_QueueFamilyCount; //Queues
	std::vector<VkQueueFamilyProperties> m_QueueFamilyProp;
	float m_QueuePriorities[2] = { 1.0f, 1.0f };
	std::vector<VkDeviceQueueCreateInfo> m_DeviceQueueCreateInfo;
	VkDeviceCreateInfo m_DeviceCreateInfo;
	std::vector<VkQueue> m_Queues;

	//CreateGLFWwindow()
	GLFWwindow* m_Window;
	int m_Width, m_Height;

	PFN_vkCreateInstance m_pfnCreateInstance; //VkFunctionPointers
	PFN_vkCreateDevice m_pfnCreateDevice;
	PFN_vkGetDeviceProcAddr m_pfnGetDeviceProcAddr;

	//CreateSurface()
	VkSurfaceKHR m_Surface; //Requires a Instance and a Window
	struct PhysicalDevice_QueueFamilies_Queues
	{
		uint32_t physicalDevice;
		uint32_t queueFamily;
		uint32_t queues;
		VkQueueFlags queueFlags;
		bool queueGraphicsBit;
		bool queueComputeBit;
		bool queueTransferBit;
		bool queueSparseBindingBit;

		VkBool32 presentSupport = false;
		void SeparateFlags()
		{
			queueGraphicsBit = queueFlags & VK_QUEUE_GRAPHICS_BIT;
			queueComputeBit = queueFlags & VK_QUEUE_COMPUTE_BIT;
			queueTransferBit = queueFlags & VK_QUEUE_TRANSFER_BIT;
			queueSparseBindingBit = queueFlags & VK_QUEUE_SPARSE_BINDING_BIT;
		}
	};
	std::vector<PhysicalDevice_QueueFamilies_Queues> m_QueueGeneralPresentSupport; //Used in CopyBuffer()

	//CreateSwapChain()
	bool m_SwapChainEXTAvailable = false;
	VkSurfaceCapabilitiesKHR m_SurfaceCompat;
	uint32_t m_SurfaceFormatCount;
	uint32_t m_SurfacePresentModeCount;
	std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
	std::vector<VkPresentModeKHR> m_SurfacePresentModes;
	bool m_UsableSwapChain = false;
	VkSurfaceFormatKHR m_ChosenSurfaceFormat;
	VkPresentModeKHR m_ChosenPresentMode;
	uint32_t m_ImageCount;
	
	VkSwapchainCreateInfoKHR m_SwapChainCreateInfo;
	VkSwapchainKHR m_SwapChain;
	std::vector<VkImage> m_SwapChainImages;
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent;

	//CreateSwapChainImages()
	std::vector<VkImageView> m_SwapChainImageViews;
	std::vector<VkImageViewCreateInfo> m_SwapChainImageViewCreateInfos;
	
	//CreateShader()
	std::string m_FilepathVert = "res/shaders/vk_basic.vert.spv";
	std::vector<char> m_ShaderBinVert;
	size_t m_FileSizeVert;
	std::string m_FilepathFrag = "res/shaders/vk_basic.frag.spv";
	std::vector<char> m_ShaderBinFrag;
	size_t m_FileSizeFrag;
	VkShaderModuleCreateInfo m_ShaderModCreateInfoVert;
	VkShaderModuleCreateInfo m_ShaderModCreateInfoFrag;
	VkShaderModule m_ShaderModVert;
	VkShaderModule m_ShaderModFrag;
	
	//CreateRenderPass()
	VkAttachmentDescription m_ColourAttachmentDesc = {};
	VkAttachmentReference m_ColourAttachmentRef = {};
	VkAttachmentDescription m_DepthAttachmentDesc = {};
	VkAttachmentReference m_DepthAttachmentRef = {};
	VkSubpassDescription m_SubpassDesc = {};
	VkSubpassDependency m_SubpassDependency = {};
	VkRenderPassCreateInfo m_RenderPassCreateInfo = {};
	VkRenderPass m_RenderPass;

	//CreateDescriptorLayout()
	std::vector<VkDescriptorSetLayoutBinding> m_DescSetLayoutBinds = {};
	VkDescriptorSetLayoutCreateInfo m_DescSetLayoutCreateInfo = {};
	VkDescriptorSetLayout m_DescSetLayout = {};

	//CreateDescriptorPool()
	std::vector<VkDescriptorPoolSize> m_PoolSizes = {};
	VkDescriptorPoolCreateInfo m_PoolCreateInfo = {};
	VkDescriptorPool m_Pool;

	//CreateDescriptorSets()
	std::vector<VkDescriptorSetLayout> m_DescSetLayouts;
	VkDescriptorSetAllocateInfo m_DescSetAllocInfo = {};
	std::vector<VkDescriptorSet> m_DescSets;
	std::vector<VkDescriptorBufferInfo> m_DescBufferInfos;
	std::vector<VkDescriptorImageInfo> m_DescImageInfos;
	std::vector<VkWriteDescriptorSet> m_WriteDescSets;

	//CreateGraphicsPipeline()
	VkPipelineShaderStageCreateInfo m_ShaderStageCreateInfoVert;
	VkPipelineShaderStageCreateInfo m_ShaderStageCreateInfoFrag;
	VkPipelineVertexInputStateCreateInfo m_PipelineVertInStateCreateInfo;
	VkPipelineInputAssemblyStateCreateInfo m_PipelineInAssemStateCreateInfo;
	VkViewport m_Viewport;
	VkRect2D m_Scissor;
	VkPipelineViewportStateCreateInfo m_PipelineViewportStateCreateInfo;
	VkPipelineRasterizationStateCreateInfo m_PipelineRasterizerStateCreateInfo;
	VkPipelineMultisampleStateCreateInfo m_PipelineMultisamplingStateCreateInfo;
	VkPipelineDepthStencilStateCreateInfo m_PipelineDepthStencilStateCreateInfo;
	VkPipelineColorBlendAttachmentState m_PipelineColorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo m_PipelineColorBlendStateCreateInfo;
	VkDynamicState m_DynamicStates[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH};
	VkPipelineDynamicStateCreateInfo m_PipelineDynamicStateCreateInfo;
	VkPipelineLayoutCreateInfo m_PipelineLayoutCreateInfo;
	VkPipelineLayout m_PipelineLayout;
	VkGraphicsPipelineCreateInfo m_GraphicPipelineCreateInfo;
	VkPipeline m_Pipeline;

	//CreateFramebuffers()
	std::vector<VkFramebuffer> m_SwapChainFramebuffer;
	std::vector<VkFramebufferCreateInfo> m_FramebufferCreateInfos;

	//CreateDepthImage()
	VkImage m_DepthImage;
	VkDeviceMemory m_DepthImageMem;
	VkImageView m_DepthImageView;

	//CreateCommandPool()
	std::vector<VkCommandPoolCreateInfo> m_CmdPoolsCreateInfo;
	std::vector<VkCommandPool> m_CmdPools;
	std::vector<VkCommandBufferAllocateInfo> m_CmdBufferAllocInfo;
	std::vector<VkCommandBuffer> m_CmdBuffers;
	std::vector<VkCommandBufferBeginInfo> m_CmdBufferBeginInfos; //Used in BeginCommandbufferRecord()

	//BeginRenderPass()
	std::vector<VkRenderPassBeginInfo> m_RenderPassBeginInfos;
	std::array<VkClearValue, 2> m_ClearColours;
	VkDeviceSize m_OffsetVert[1] = { 0 };
	
	//CreateBuffer()
	std::vector<VkBufferCreateInfo> m_BufferCreateInfos;
	std::vector<VkMemoryRequirements> m_MemRequirements;
	std::vector<VkMemoryAllocateInfo> m_MemAllocInfos;

	//CreateImage() - CreateTexture() - CreateImageView()
	std::string m_FilepathTex = "res/img/andrew_manga_3_square.png";
	VkDeviceSize m_TexSize;
	std::vector<VkImageCreateInfo> m_ImageCreateInfos = {};
	std::vector<VkImageViewCreateInfo> m_TexImageViewCreateInfos = {}; //Needs to be std::vector-ed
	VkImageView m_TexImageView;

	//CreateSmapler()
	VkSamplerCreateInfo m_SamplerCreateInfo = {};
	VkSampler m_Sampler;

	//VBO, IBO and UBO structures
	struct VertexAttribBinding
	{
		uint32_t vertexBindingDescriptionCount = 0;
		VkVertexInputBindingDescription* pVertexBindingDescriptions = nullptr;
		uint32_t vertexAttributeDescriptionCount = 0;
		VkVertexInputAttributeDescription* pVertexAttributeDescriptions = nullptr;
		uint64_t verticesSize = 0;
		uint32_t verticesCount = 0;
		float* pVertexData = nullptr;
	};
	VertexAttribBinding m_VertexData;
	struct IndexData
	{
		uint64_t verticesSize = 0;
		uint32_t verticesCount = 0;
		uint16_t* pIndexData = nullptr;
	};
	IndexData m_IndexData;

	struct UniformBufferObject
	{
		glm::mat4 proj;
		glm::mat4 view;
		glm::mat4 modl;
		uint32_t currentImage;
	};
	UniformBufferObject m_UBO;
	
	//CreateTextureImage()
	VkImage m_TexImage;
	VkDeviceMemory m_TexBufferMem;
	VkBuffer m_StagingTexBuffer;
	VkDeviceMemory m_StagingTexBufferMem;

	//CreateVBO()
	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMem;
	VkBuffer m_StagingVertexBuffer;
	VkDeviceMemory m_StagingVertexBufferMem;

	//CreateIBO()
	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMem;
	VkBuffer m_StagingIndexBuffer;
	VkDeviceMemory m_StagingIndexBufferMem;

	//CreateUBO()
	std::vector<VkBuffer> m_UniformBuffers;
	std::vector<VkDeviceMemory> m_UniformBuffersMem;
	
	//CreateSemaphoresAndFences()
	const int MAX_FRAMES_IN_FLIGHT = 2;
	VkSemaphoreCreateInfo m_SemaphoreCreateInfo;
	VkFenceCreateInfo m_FenceCreateInfo;
	std::vector<VkSemaphore> m_ImageAvailableSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;
	bool m_FramebufferResized = false;

public:
	Vulkan(int width, int height, const VertexAttribBinding& vertexdata, const IndexData& indexdata);
	~Vulkan();

private:
	void CreateInstance();
	void DestroyInstance();

	void CreateDevice();
	void DestroyDevice();

	void CreateGFLWwindow();
	void DestroyGLFWwindow();
	
	void CreateSurface();
	void DestroySurface();

	void CreateSwapChain();
	void DestroySwapChain();

	void CreateSwapChainImageViews();
	void DestroySwapChainImageViews();

	void CreateShader();
	void DestroyShader();
	
	void CreateRenderPass();
	void DestroyRenderPass();

	void CreateDescriptorLayout();
	void DestroyDescriptorLayout();

	void CreateGraphicPipeline();
	void DestroyGraphicPipeline();

	void CreateFramebuffers();
	void DestroyFramebuffers();

	void CreateCommandPool();
	void DestroyCommandPool();

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & imageMemory);
	VkCommandBuffer BeginSingleTimeCommand();
	void EndSingleTimeCommand(VkCommandBuffer cmdBuffer);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat FindDepthFormat();
	bool HasStencilComponent(VkFormat format);
	
	void CreateDepthImage();
	void DestroyDepthImage();

	void CreateTextureImage();
	void DestroyTextureImage();

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void CreateTextureImageView();
	void DestroyTextureImageView();

	void CreateTextureSampler();
	void DestroyTextureSampler();

	void CreateVertexBuffer();
	void DestroyVertexBuffer();

	void CreateIndexBuffer();
	void DestroyIndexBuffer();

	void CreateUniformBuffer();
	void DestroyUniformBuffer();

	void CreateDiscriptorPool();
	void DestroyDescriptionorPool();

	void CreateDescriptorSets();
	void DestroyDescriptorSets();

	void CreateCommandbuffers();
	void DestroyCommandbuffers();

	void CreateSemaphoresAndFences();
	void DestroySemaphoresAndFences();

public:
	void BeignCommandbufferRecord();
	void EndCommandbufferRecord();

	void BeginRenderPass();
	void EndRenderPass();
	
	void Update();
	void UpdateUniformBuffers();

	void RecreateSwapChain()
	{
		//Check if window minimised
		int glfwExtentWidth = 0, glfwExtentHeight = 0; 
		while (glfwExtentWidth == 0 || glfwExtentHeight == 0)
		{
			glfwGetFramebufferSize(m_Window, &glfwExtentWidth, &glfwExtentHeight);
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(m_Device);
		
		//Recreate the SwapChain and Dependent structures
		DestroyCommandbuffers();
		DestroyFramebuffers();
		DestroyDepthImage();
		DestroyGraphicPipeline();
		DestroyRenderPass();
		DestroySwapChainImageViews();
		DestroySwapChain();

		CreateSwapChain();
		CreateSwapChainImageViews();
		CreateRenderPass();
		CreateGraphicPipeline();
		CreateDepthImage();
		CreateFramebuffers();
		CreateCommandbuffers();

		//Resubmitted the CmdBuffer and RenderPass
		BeignCommandbufferRecord();
		BeginRenderPass();
		EndRenderPass();
		EndCommandbufferRecord();
	}

private:
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	
	VkExtent2D ChooseNewSwapChainExtent();

	friend std::ostream& operator<< (std::ostream& stream, const VkLayerProperties& properties);
	void PrintAllVkLayerProperties(const VkLayerProperties* properties, size_t count);

	friend std::ostream& operator<< (std::ostream& stream, const VkExtensionProperties& properties);
	void PrintAllVkExtensionProperties(const VkExtensionProperties* properties, size_t count);

	friend std::ostream& operator<< (std::ostream& stream, const VkPhysicalDeviceProperties& properties);
	void PrintVkPhysicalDeviceFeatures(const VkPhysicalDeviceFeatures& properties);

	friend std::ostream& operator<< (std::ostream& stream, const VkPhysicalDeviceFeatures& properties);
	void PrintVkPhysicalDeviceProperties(const VkPhysicalDeviceProperties& properties);

	friend std::ostream& operator<< (std::ostream& stream, const VkPhysicalDeviceMemoryProperties& properties);
	void PrintVkPhysicalDeviceMemoryProperties(const VkPhysicalDeviceMemoryProperties& properties);

	friend std::ostream& operator<< (std::ostream& stream, const VkQueueFamilyProperties& properties);
	void PrintAllVkQueueFamilyProperties(const VkQueueFamilyProperties* properties, size_t count);

public:
	
};