#pragma once

#include "vulkan/vulkan.h"
#include "../Window.h"

class UniformBuffer
{
private:
	const Window& m_Window;

	VkBufferCreateInfo m_BufferCreateInfo = {};
	VkMemoryRequirements m_MemRequirement;
	VkMemoryAllocateInfo m_MemAllocInfo;

	std::vector<VkBuffer> m_UniformBuffers;
	std::vector<VkDeviceMemory> m_UniformBuffersMem;
	VkDeviceSize m_Size;

public:
	UniformBuffer(const Window& window, VkDeviceSize size);
	~UniformBuffer();
	
	void UpdateUniformBuffers(void* data, unsigned int currentImage);
	inline const std::vector<VkBuffer>& GetUniformBuffer() const { return m_UniformBuffers; }
	inline VkDeviceSize GetSize() const { return m_Size; }
private:
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory);
};