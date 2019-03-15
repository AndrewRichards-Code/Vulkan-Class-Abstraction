#pragma once
#include "vulkan/vulkan.h"
#include "../../Window.h"
#include "../../shader/Shader.h"
#include "../../texture/Texture.h"
#include "../../buffer/UniformBuffer.h"


namespace Descriptor
{
class Pool
{
private:
	const Window& m_Window;

	VkDescriptorPool m_Pool;
	std::vector<VkDescriptorPoolSize> m_PoolSizes;
	VkDescriptorPoolCreateInfo m_PoolCreateInfo;

public:
	Pool(const Window& window, const std::vector<VkDescriptorType>& types);
	~Pool();

	inline VkDescriptorPool GetDescriptorPool() const {return m_Pool;}
};

class Sets
{
private:
	const Window& m_Window;
	const Shader& m_Shader;
	size_t m_Num;

	std::vector<VkDescriptorSetLayout> m_DescSetLayouts;
	VkDescriptorSetAllocateInfo m_DescSetAllocInfo = {};

	std::vector<VkDescriptorSet> m_DescSets;
	std::vector<VkDescriptorBufferInfo> m_DescBufferInfos;
	std::vector<VkDescriptorImageInfo> m_DescImageInfos;
	std::vector<VkWriteDescriptorSet> m_WriteDescSets;

public:
	Sets(const Window& window, const Pool& pool, const Shader& shader);
	~Sets();
	void AddUniformBuffer(const UniformBuffer& ubo, int bindingIndex);
	void AddTexture(const Texture& texture, int bindingIndex);
	void Update();

	inline const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_DescSets; }
};
}