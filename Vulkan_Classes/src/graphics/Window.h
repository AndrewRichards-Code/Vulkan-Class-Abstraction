#pragma once

#include <vector>
#include <algorithm>

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"

#include "Debug.h"

class Window
{
private:
	//CreateInstance()
	VkInstance m_Instance;
	VkApplicationInfo m_AppInfo;
	VkInstanceCreateInfo m_InstanceInfo;

	uint32_t m_Inst_LayerCount = 0; //Layer and Extensions
	uint32_t m_Inst_ExtCount = 0;
	std::vector<VkLayerProperties> m_Inst_LayerProp;
	std::vector<VkExtensionProperties> m_Inst_ExtProp;
	std::vector<const char*> m_Inst_LayersStrings = { "VK_LAYER_LUNARG_standard_validation" };
	std::vector<const char*> m_Inst_ExtStrings = {}; //GLFW extenstion are added.

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

public:
	bool m_FramebufferResized = false;

public:
	Window(int width, int height);
	~Window();
	void Update();

	inline GLFWwindow* GetGLFWwindow() const { return m_Window; }
	inline float GetRatio() const { return (float)m_Width / (float)m_Height; }
	inline const VkDevice& GetDevice() const { return m_Device; };
	inline const std::vector<VkPhysicalDevice>& GetPhysicalDevices() const { return m_PhysicalDevices; }
	inline const VkPhysicalDeviceMemoryProperties& GetPhysicalDeviceMemoryProperties() const { return m_DeviceMemProp; }
	inline const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() const { return m_DeviceFeatures; }
	inline const VkSwapchainKHR& GetSwapChain() const { return m_SwapChain; }
	inline const std::vector<VkQueue>& GetQueues() const { return m_Queues; }
	inline const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const { return m_QueueFamilyProp; }
	inline const std::vector<VkDeviceQueueCreateInfo>& GetDeviceQueueCreateInfo() const { return m_DeviceQueueCreateInfo; }
	inline const std::vector<PhysicalDevice_QueueFamilies_Queues>& GetQueueGeneralPresentSupport() const{return m_QueueGeneralPresentSupport;}
	inline const VkFormat& GetSwapChainImageFormat() const { return m_SwapChainImageFormat;}
	inline const VkExtent2D& GetSwapChainExtent() const { return m_SwapChainExtent; }
	inline const std::vector<VkImageView>& GetSwapChainImageViews() const { return m_SwapChainImageViews; }

private:
	void CreateInstance();
	void DestroyInstance();

	void CreateDevice();
	void DestroyDevice();

	void CreateGLFWwindow();
	void DestroyGLFWwindow();

	void CreateSurface();
	void DestroySurface();

	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	VkExtent2D ChooseNewSwapChainExtent();
	inline void SetWidthHeight(int width, int height) 
	{
		m_Width = width;
		m_Height = height;
	}

public:
	void CreateSwapChain();
	void DestroySwapChain();

	void CreateSwapChainImageViews();
	void DestroySwapChainImageViews();

};

