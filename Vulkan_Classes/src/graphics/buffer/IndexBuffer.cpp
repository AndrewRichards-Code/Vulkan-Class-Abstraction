#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const Window& window, const Cmd::Pool& cmdPool, VkDeviceSize size, void* data)
	:m_Window(window), m_CmdPool(cmdPool), m_Size(size)
{
	//Vertexbuffer CPU Side
	CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingBuffer, m_StagingBufferMem);

	void* bufferData;
	VkResult result = vkMapMemory(m_Window.GetDevice(), m_StagingBufferMem, 0, size, 0, &bufferData);
	VkAssert(result);
	memcpy(bufferData, data, static_cast<size_t>(size));
	vkUnmapMemory(m_Window.GetDevice(), m_StagingBufferMem);

	//VertexBuffer GPU Side
	CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMem);

	CopyBuffer(m_StagingBuffer, m_Buffer, size);
	vkFreeMemory(m_Window.GetDevice(), m_StagingBufferMem, nullptr);
	vkDestroyBuffer(m_Window.GetDevice(), m_StagingBuffer, nullptr);
}

IndexBuffer::~IndexBuffer()
{
	vkFreeMemory(m_Window.GetDevice(), m_BufferMem, nullptr);
	vkDestroyBuffer(m_Window.GetDevice(), m_Buffer, nullptr);
}

void IndexBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	std::vector<uint32_t>queueFamilyIndices; //TODO: Look into this further
	queueFamilyIndices.reserve(m_Window.GetQueueFamilyProperties().size());
	for (int i = 0; i < static_cast<int32_t>(m_Window.GetQueueFamilyProperties().size()); i++)
	{
		queueFamilyIndices.push_back(i);
	}

	m_BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	m_BufferCreateInfo.pNext = nullptr;
	m_BufferCreateInfo.flags = 0;
	m_BufferCreateInfo.size = size;
	m_BufferCreateInfo.usage = usage;
	m_BufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	m_BufferCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(m_Window.GetQueueFamilyProperties().size());
	m_BufferCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();

	VkResult result = vkCreateBuffer(m_Window.GetDevice(), &m_BufferCreateInfo, nullptr, &buffer);
	VkAssert(result);

	vkGetBufferMemoryRequirements(m_Window.GetDevice(), buffer, &m_MemRequirement);

	std::vector<bool> foundSuitableMemType(m_Window.GetPhysicalDeviceMemoryProperties().memoryTypeCount, false);
	bool foundGeneral = false;
	for (uint32_t i = 0; i < static_cast<uint32_t>(foundSuitableMemType.size()); i++)
	{
		if ((m_MemRequirement.memoryTypeBits & (1 << i))
			&& (m_Window.GetPhysicalDeviceMemoryProperties().memoryTypes[i].propertyFlags & properties)
			== properties)
		{
			foundSuitableMemType[i] = true;
			foundGeneral = true;
		}
	}
	if (!foundGeneral)
		std::cout << "Vulkan could not find a suitable memory for the allocation of the buffer!" << std::endl;

	uint32_t memoryTypeIndex;
	for (auto index : foundSuitableMemType)
	{
		if (index == true) //Pick the first valid;
		{
			memoryTypeIndex = foundSuitableMemType.at(index);
			break;
		}
	}

	m_MemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	m_MemAllocInfo.pNext = nullptr;
	m_MemAllocInfo.allocationSize = m_MemRequirement.size;
	m_MemAllocInfo.memoryTypeIndex = memoryTypeIndex;

	result = vkAllocateMemory(m_Window.GetDevice(), &m_MemAllocInfo, nullptr, &bufferMemory);
	VkAssert(result);

	result = vkBindBufferMemory(m_Window.GetDevice(), buffer, bufferMemory, 0);
	VkAssert(result);
}

void IndexBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	uint32_t cmdPoolIndex = 0;
	for (auto& queueGeneralSupport : m_Window.GetQueueGeneralPresentSupport())
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
	cmdBufferAllocInfo.commandPool = m_CmdPool.GetCmdPool()[cmdPoolIndex];
	cmdBufferAllocInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuffer;
	VkResult result = vkAllocateCommandBuffers(m_Window.GetDevice(), &cmdBufferAllocInfo, &cmdBuffer);
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

	vkQueueSubmit(m_Window.GetQueues()[cmdPoolIndex], 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_Window.GetQueues()[cmdPoolIndex]);

	vkFreeCommandBuffers(m_Window.GetDevice(), m_CmdPool.GetCmdPool()[cmdPoolIndex], 1, &cmdBuffer);
}