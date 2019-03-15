#pragma once
#include "Vulkan.h"
#include <chrono>

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

class HelloTriangle
{
private:
	std::unique_ptr<Vulkan> m_VulkanContext;

	VkSubmitInfo m_SubmitInfo = {};
	VkPresentInfoKHR m_PresentInfo = {};

	uint32_t m_ImageIndex;
	size_t m_CurrentFrame = 0;

	struct VertexData
	{
		glm::vec3 position;
		glm::vec3 colour;
		glm::vec2 texCoord;
	};
	std::vector<VertexData> m_Vertices;
	VkVertexInputBindingDescription m_VertexBindingDesc = {};
	std::vector<VkVertexInputAttributeDescription> m_VertexAttribDesc;
	Vulkan::VertexAttribBinding m_VertexData;
	
	std::vector<uint16_t> m_Indices;
	Vulkan::IndexData m_IndexData;

	Vulkan::UniformBufferObject m_UBO;

public:
	HelloTriangle(int width, int height);
	~HelloTriangle();

	void Run();

private:
	void DrawFrame();

	void UpdateUBO(uint32_t currentImage);

	void SetupVertexBuffer();

};

