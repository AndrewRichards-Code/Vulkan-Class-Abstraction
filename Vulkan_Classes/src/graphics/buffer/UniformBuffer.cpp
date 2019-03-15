#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(const Window& window, VkDeviceSize size)
	:m_Window(window), m_Size(size)
{
	m_UniformBuffers.resize(m_Window.GetSwapChainImageViews().size());
	m_UniformBuffersMem.resize(m_Window.GetSwapChainImageViews().size());

	for (int i = 0; i < static_cast<int32_t>(m_Window.GetSwapChainImageViews().size()); i++)
	{
		CreateBuffer(m_Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i], m_UniformBuffersMem[i]);
	}
}
UniformBuffer::~UniformBuffer()
{
	for (size_t i = 0; i < m_Window.GetSwapChainImageViews().size(); i++)
	{
		vkDestroyBuffer(m_Window.GetDevice(), m_UniformBuffers[i], nullptr);
		vkFreeMemory(m_Window.GetDevice(), m_UniformBuffersMem[i], nullptr);
	}
}

void UniformBuffer::UpdateUniformBuffers(void* data, unsigned int currentImage)
{
	void* uniformData;
	VkResult result = vkMapMemory(m_Window.GetDevice(), m_UniformBuffersMem[currentImage], 0, m_Size, 0, &uniformData);
	VkAssert(result);
	memcpy(uniformData, data, static_cast<size_t>(m_Size));
	vkUnmapMemory(m_Window.GetDevice(), m_UniformBuffersMem[currentImage]);
}

void UniformBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
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