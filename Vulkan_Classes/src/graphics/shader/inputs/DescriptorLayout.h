#pragma once

#include <vector>
#include "vulkan/vulkan.h"

namespace Descriptor
{
struct Layout
{
private:
	std::vector<VkDescriptorSetLayoutBinding> m_DescriptorSetLayoutBindings;

public:
	Layout() {};

	void AddBinding(int binding, VkDescriptorType type, VkShaderStageFlagBits stage, int count = 1, VkSampler* immutableSamplers = nullptr)
	{
		VkDescriptorSetLayoutBinding temp;
		temp.binding = binding;
		temp.descriptorType = type;
		temp.descriptorCount = count;
		temp.stageFlags = stage;
		temp.pImmutableSamplers = immutableSamplers;

		m_DescriptorSetLayoutBindings.push_back(temp);
	}

	const std::vector<VkDescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings()const { return m_DescriptorSetLayoutBindings; }
	const VkDescriptorSetLayoutBinding* GetDescriptorSetLayoutBindingsPtr()const { return m_DescriptorSetLayoutBindings.data(); }
};
}