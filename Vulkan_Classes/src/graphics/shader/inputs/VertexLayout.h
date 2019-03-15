#pragma once

#include <vector>
#include "vulkan/vulkan.h"

#define VK_FLOAT VK_FORMAT_R32_SFLOAT
#define VK_DOUBLE VK_FORMAT_R64_SFLOAT

#define VK_VEC2  VK_FORMAT_R32G32_SFLOAT
#define VK_VEC3  VK_FORMAT_R32G32B32_SFLOAT
#define VK_VEC4  VK_FORMAT_R32G32B32A32_SFLOAT

#define VK_IVEC2  VK_FORMAT_R32G32_SINT
#define VK_IVEC3  VK_FORMAT_R32G32B32_SINT
#define VK_IVEC4  VK_FORMAT_R32G32B32A32_SINT

#define VK_UVEC2  VK_FORMAT_R32G32_UINT
#define VK_UVEC3  VK_FORMAT_R32G32B32_UINT
#define VK_UVEC4  VK_FORMAT_R32G32B32A32_UINT

struct VertexLayout
{
private:
	VkVertexInputBindingDescription m_VertexBindingDesc = {};
	std::vector<VkVertexInputAttributeDescription> m_VertexAttribDesc;

	void Update()
	{
		vertexBindingDescriptionCount = 1;
		vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexAttribDesc.size());
		pVertexBindingDescriptions = &m_VertexBindingDesc;
		pVertexAttributeDescriptions = m_VertexAttribDesc.data();
	}
	
public:
	VertexLayout(int stride)
	{
		m_VertexBindingDesc.binding = 0; //VBO Index
		m_VertexBindingDesc.stride = stride;
		m_VertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		Update();
	}
	void AddAttribute(int binding, int location, VkFormat format, int offset)
	{
		VkVertexInputAttributeDescription temp = {};
		temp.binding = binding;
		temp.location = location; //location in vertex shader
		temp.format = format;
		temp.offset = offset;
		m_VertexAttribDesc.push_back(temp);
		Update();
	}
	
	uint32_t vertexBindingDescriptionCount = 0;
	uint32_t vertexAttributeDescriptionCount = 0;
	VkVertexInputBindingDescription* pVertexBindingDescriptions = nullptr;
	VkVertexInputAttributeDescription* pVertexAttributeDescriptions = nullptr;
};
