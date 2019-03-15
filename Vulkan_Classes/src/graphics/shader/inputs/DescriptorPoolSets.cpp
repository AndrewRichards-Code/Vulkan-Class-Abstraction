#include "DescriptorPoolSets.h"

using namespace Descriptor;

Pool::Pool(const Window& window, const std::vector<VkDescriptorType>& types)
	:m_Window(window)
{
	size_t descriptorCount = m_Window.GetSwapChainImageViews().size();
	m_PoolSizes.resize(types.size());
	for(int i = 0; i < types.size(); i++)
	{
		m_PoolSizes[i].type = types[i];
		m_PoolSizes[i].descriptorCount = static_cast<uint32_t>(descriptorCount);
	}

	m_PoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	m_PoolCreateInfo.poolSizeCount = static_cast<uint32_t>(m_PoolSizes.size());
	m_PoolCreateInfo.pPoolSizes = m_PoolSizes.data();
	m_PoolCreateInfo.maxSets = static_cast<uint32_t>(m_Window.GetSwapChainImageViews().size());

	VkResult result = vkCreateDescriptorPool(m_Window.GetDevice(), &m_PoolCreateInfo, nullptr, &m_Pool);
	VkAssert(result);
}

Pool::~Pool()
{
	vkDestroyDescriptorPool(m_Window.GetDevice(), m_Pool, nullptr);
}


Sets::Sets(const Window& window, const Pool& pool, const Shader& shader)
	:m_Window(window), m_Shader(shader)
{
	m_Num = m_Window.GetSwapChainImageViews().size();
	m_DescSetLayouts.reserve(m_Num);
	for (size_t i = 0; i < m_Window.GetSwapChainImageViews().size(); i++)
	{
		m_DescSetLayouts.push_back(m_Shader.GetDescriptorSetLayout());
	}

	m_DescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	m_DescSetAllocInfo.descriptorPool = pool.GetDescriptorPool();
	m_DescSetAllocInfo.descriptorSetCount = static_cast<uint32_t>(m_Window.GetSwapChainImageViews().size());
	m_DescSetAllocInfo.pSetLayouts = m_DescSetLayouts.data();

	m_DescSets.resize(m_Num);
	VkResult result = vkAllocateDescriptorSets(m_Window.GetDevice(), &m_DescSetAllocInfo, m_DescSets.data());
	VkAssert(result);

	m_DescBufferInfos.reserve(m_Num);
	m_DescImageInfos.reserve(m_Num);
	m_WriteDescSets.reserve(m_Num);
}

Sets::~Sets()
{

}

void Sets::AddUniformBuffer(const UniformBuffer& ubo, int bindingIndex)
{
	for (int i = 0; i < m_Num; i++)
	{
		VkDescriptorBufferInfo tempDescBufferInfo = {};
		tempDescBufferInfo.buffer = ubo.GetUniformBuffer()[i];
		tempDescBufferInfo.offset = 0;
		tempDescBufferInfo.range = ubo.GetSize();
		m_DescBufferInfos.push_back(tempDescBufferInfo);

		VkWriteDescriptorSet tempWriteDescSet = {};
		tempWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		tempWriteDescSet.dstSet = m_DescSets[i];
		tempWriteDescSet.dstBinding = bindingIndex; //Index in Vertex shader
		tempWriteDescSet.dstArrayElement = 0; //Nou using an array
		tempWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		tempWriteDescSet.descriptorCount = 1;
		tempWriteDescSet.pBufferInfo = &m_DescBufferInfos.back();
		tempWriteDescSet.pImageInfo = nullptr;
		tempWriteDescSet.pTexelBufferView = nullptr;
		m_WriteDescSets.push_back(tempWriteDescSet);
	}
}

void Sets::AddTexture(const Texture& texture, int bindingIndex)
{
	for (int i = 0; i < m_Num; i++)
	{
		VkDescriptorImageInfo tempDescImageInfo = {};
		tempDescImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		tempDescImageInfo.imageView = texture.GetImage()->GetImageView();
		tempDescImageInfo.sampler = texture.GetSampler();
		m_DescImageInfos.push_back(tempDescImageInfo);

		VkWriteDescriptorSet tempWriteDescSet = {};
		tempWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		tempWriteDescSet.dstSet = m_DescSets[i];
		tempWriteDescSet.dstBinding = bindingIndex; //Index in Fragment shader
		tempWriteDescSet.dstArrayElement = 0; //Nou using an array
		tempWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		tempWriteDescSet.descriptorCount = 1;
		tempWriteDescSet.pBufferInfo = nullptr;
		tempWriteDescSet.pImageInfo = &m_DescImageInfos.back();
		tempWriteDescSet.pTexelBufferView = nullptr;
		m_WriteDescSets.push_back(tempWriteDescSet);
	}
}

void Sets::Update()
{
	vkUpdateDescriptorSets(m_Window.GetDevice(), static_cast<uint32_t>(m_WriteDescSets.size()), m_WriteDescSets.data(), 0, nullptr);
}

