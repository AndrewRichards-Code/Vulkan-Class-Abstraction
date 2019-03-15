#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const std::string& filepath, const Cmd::Pool& cmdPool)
	:m_Filepath(filepath), m_Window(cmdPool.GetWindow()), m_CmdPool(cmdPool)
{
	unsigned char* pixelData = stbi_load(m_Filepath.c_str(), &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);
	m_TexSize = m_Width * m_Height * STBI_rgb_alpha;

	CreateBuffer(m_TexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingTexBuffer, m_StagingTexBufferMem);

	void* texData;
	VkResult result = vkMapMemory(m_Window.GetDevice(), m_StagingTexBufferMem, 0, m_TexSize, 0, &texData);
	VkAssert(result);
	memcpy(texData, pixelData, static_cast<size_t>(m_TexSize));
	vkUnmapMemory(m_Window.GetDevice(), m_StagingTexBufferMem);

	stbi_image_free(pixelData);

	VkExtent3D extent = { (uint32_t)m_Width, (uint32_t)m_Height, (uint32_t)1 };
	m_Image = std::make_shared<Image>(m_Window, extent, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

	TransitionImageLayout(m_Image->GetImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(m_StagingTexBuffer, m_Image->GetImage(), static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));
	TransitionImageLayout(m_Image->GetImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	CreateTextureSampler();
}

Texture::~Texture()
{
	DestroyTextureSampler();
}

void Texture::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
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

void Texture::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer cmdBuffer = Cmd::SingleTimeCommand::Begin(m_CmdPool);

	VkBufferImageCopy imageCopy = {};
	imageCopy.bufferOffset = 0;
	imageCopy.bufferRowLength = 0;
	imageCopy.bufferImageHeight = 0;
	imageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.imageSubresource.mipLevel = 0;
	imageCopy.imageSubresource.baseArrayLayer = 0;
	imageCopy.imageSubresource.layerCount = 1;
	imageCopy.imageOffset = { 0, 0, 0 };
	imageCopy.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(cmdBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

	Cmd::SingleTimeCommand::End(m_CmdPool, cmdBuffer);
}

void Texture::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer cmdBuffer = Cmd::SingleTimeCommand::Begin(m_CmdPool);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (HasStencilComponent(format))
		{
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;


	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
	{
		std::cout << "Vulkan can't use the described layout transition" << std::endl;
	}

	vkCmdPipelineBarrier(
		cmdBuffer,
		sourceStage,
		destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	Cmd::SingleTimeCommand::End(m_CmdPool, cmdBuffer);
}

bool Texture::HasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Texture::CreateTextureSampler()
{
	m_SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	m_SamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	m_SamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	m_SamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	m_SamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	m_SamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	if (m_Window.GetPhysicalDeviceFeatures().samplerAnisotropy == 1)
	{
		m_SamplerCreateInfo.anisotropyEnable = VK_TRUE;
		m_SamplerCreateInfo.maxAnisotropy = 16;
	}
	else
	{
		m_SamplerCreateInfo.anisotropyEnable = VK_FALSE;
		m_SamplerCreateInfo.maxAnisotropy = 1;
	}
	m_SamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	m_SamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	m_SamplerCreateInfo.compareEnable = VK_FALSE;
	m_SamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	m_SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	m_SamplerCreateInfo.mipLodBias = 0.0f;
	m_SamplerCreateInfo.minLod = 0.0f;
	m_SamplerCreateInfo.maxLod = 0.0f;

	VkResult result = vkCreateSampler(m_Window.GetDevice(), &m_SamplerCreateInfo, nullptr, &m_Sampler);
	VkAssert(result);
}

void Texture::DestroyTextureSampler()
{
	vkDestroySampler(m_Window.GetDevice(), m_Sampler, nullptr);
}