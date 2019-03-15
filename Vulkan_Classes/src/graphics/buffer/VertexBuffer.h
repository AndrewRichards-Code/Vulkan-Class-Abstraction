#pragma once

#include "vulkan/vulkan.h"
#include "../Window.h"
#include "../cmd/CmdPool.h"

class VertexBuffer
{
private:
	const Window& m_Window;
	const Cmd::Pool m_CmdPool;

	VkBufferCreateInfo m_BufferCreateInfo = {};
	VkMemoryRequirements m_MemRequirement;
	VkMemoryAllocateInfo m_MemAllocInfo;

	VkBuffer m_StagingBuffer;
	VkDeviceMemory m_StagingBufferMem;
	VkBuffer m_Buffer;
	VkDeviceMemory m_BufferMem;

public:
	VertexBuffer(const Window& window, const Cmd::Pool& cmdPool, VkDeviceSize size, void* data);
	~VertexBuffer();

	inline const VkBuffer& GetBuffer() const { return m_Buffer; }

private:
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};