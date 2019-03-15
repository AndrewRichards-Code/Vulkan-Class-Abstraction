#pragma once

#include <iostream>

#include "vulkan/vulkan.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define ASSERT(x) if(!(x)) __debugbreak();
#define VkAssert(x) ASSERT(VkLogCall(__FILE__, __LINE__, x));

static bool VkLogCall(const char* file, int line, VkResult x)
{
	if (x != VK_SUCCESS)
	{
		std::cout << "[Vulkan Error] (" << x << "): " << ", " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

#define GLFWAssert(x) ASSERT(GLFWLogCall(__FILE__, __LINE__, x));

static bool GLFWLogCall(const char* file, int line, int x)
{
	if (x != GLFW_TRUE)
	{
		std::cout << "[GLFW Error] (" << x << "): " << ", " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

static std::ostream& operator<< (std::ostream& stream, const VkLayerProperties& properties)
{
	stream << "Name: " << properties.layerName << std::endl;
	stream << "Specification Version: " << properties.specVersion << std::endl;
	stream << "Implementation Version: " << properties.implementationVersion << std::endl;
	stream << "Description: " << properties.description << std::endl;
	return stream;
}
static void PrintAllVkLayerProperties(const VkLayerProperties* properties, size_t count)
{
	std::cout << "Number of Layers: " << count << std::endl;
	for (int i = 0; i < (int)count; i++)
	{
		std::cout << "Layer " << i + 1 << std::endl;
		std::cout << properties[i] << std::endl;
	}
}

static std::ostream& operator<< (std::ostream& stream, const VkExtensionProperties& properties)
{
	stream << "Name: " << properties.extensionName << std::endl;
	stream << "Specification Version: " << properties.specVersion << std::endl;
	return stream;
}
static void PrintAllVkExtensionProperties(const VkExtensionProperties* properties, size_t count)
{
	std::cout << "Number of Extensions: " << count << std::endl;
	for (int i = 0; i < (int)count; i++)
	{
		std::cout << "Extension " << i + 1 << std::endl;
		std::cout << properties[i] << std::endl;
	}
}

static std::ostream& operator<< (std::ostream& stream, const VkPhysicalDeviceProperties& properties)
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
static void PrintVkPhysicalDeviceProperties(const VkPhysicalDeviceProperties& properties)
{
	std::cout << properties << std::endl;
}

static std::ostream& operator<< (std::ostream& stream, const VkPhysicalDeviceFeatures& properties)
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
static void PrintVkPhysicalDeviceFeatures(const VkPhysicalDeviceFeatures& features)
{
	std::cout << features << std::endl;
}

static std::ostream& operator<< (std::ostream& stream, const VkPhysicalDeviceMemoryProperties& properties)
{
	stream << "Memory Type Count: " << properties.memoryTypeCount << std::endl;
	for (int i = 0; i < static_cast<int32_t>(properties.memoryTypeCount); i++)
		stream << i << " Memory Flag: " << properties.memoryTypes[i].propertyFlags << ", Heap Index: " << properties.memoryTypes[i].heapIndex << std::endl;

	stream << "Memory Heap Count: " << properties.memoryHeapCount << std::endl;
	for (int i = 0; i < static_cast<int32_t>(properties.memoryHeapCount); i++)
		stream << i << " Heap Size: " << properties.memoryHeaps[i].size << "B, Heap Flag: " << properties.memoryHeaps[i].flags << std::endl;

	return stream;
}
static void PrintVkPhysicalDeviceMemoryProperties(const VkPhysicalDeviceMemoryProperties& properties)
{
	std::cout << properties << std::endl;
}

static std::ostream& operator<< (std::ostream& stream, const VkQueueFamilyProperties& properties)
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
static void PrintAllVkQueueFamilyProperties(const VkQueueFamilyProperties* properties, size_t count)
{
	std::cout << "Number of Queue Families: " << count << std::endl;
	for (int i = 0; i < (int)count; i++)
	{
		std::cout << "Queue Family " << i + 1 << std::endl;
		std::cout << properties[i] << std::endl;
	}
}
