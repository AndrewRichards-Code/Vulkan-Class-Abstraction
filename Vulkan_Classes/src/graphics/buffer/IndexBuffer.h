#pragma once

#include "vulkan/vulkan.h"
#include "../Window.h"
#include "../cmd/CmdPool.h"

class IndexBuffer
{
private:
	const Window& m_Window;
	const Cmd::Pool m_CmdPool;

	VkDeviceSize m_Size;
	VkBufferCreateInfo m_BufferCreateInfo = {};
	VkMemoryRequirements m_MemRequirement;
	VkMemoryAllocateInfo m_MemAllocInfo;

	VkBuffer m_StagingBuffer;
	VkDeviceMemory m_StagingBufferMem;
	VkBuffer m_Buffer;
	VkDeviceMemory m_BufferMem;

public:
	IndexBuffer(const Window& window, const Cmd::Pool& cmdPool, VkDeviceSize size, void* data);
	~IndexBuffer();

	inline const VkBuffer& GetBuffer() const { return m_Buffer; }
	inline const uint32_t GetCount() const { return static_cast<uint32_t>(m_Size); }

private:
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};