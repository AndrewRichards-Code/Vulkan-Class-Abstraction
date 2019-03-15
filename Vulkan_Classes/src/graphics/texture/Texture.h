#pragma once
#include <vector>
#include <memory>
#include "vulkan/vulkan.h"
#include "../Window.h"
#include "../texture/Image.h"
#include "../cmd/CmdBufferSingleUse.h"

class Texture
{
private:
	int m_Width, m_Height, m_Channels;
	std::string m_Filepath;
	const Window& m_Window;
	const Cmd::Pool& m_CmdPool;

	VkDeviceSize m_TexSize;
	VkBuffer m_StagingTexBuffer;
	VkDeviceMemory m_StagingTexBufferMem;

	std::shared_ptr<Image> m_Image;

	//CreateBuffer
	VkBufferCreateInfo m_BufferCreateInfo = {};
	VkMemoryRequirements m_MemRequirement;
	VkMemoryAllocateInfo m_MemAllocInfo;

	VkSamplerCreateInfo m_SamplerCreateInfo = {};
	VkSampler m_Sampler;

public:
	Texture(const std::string& filepath, const Cmd::Pool& cmdPool);
	~Texture();

	inline const std::shared_ptr<Image> GetImage() const { return m_Image; }
	inline const VkSampler& GetSampler() const { return m_Sampler; }

private:
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	bool HasStencilComponent(VkFormat format);
	void CreateTextureSampler();
	void DestroyTextureSampler();
};

