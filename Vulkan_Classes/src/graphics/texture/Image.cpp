#include "Image.h"

Image::Image(const Window& window, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectFlags, VkImageTiling tiling, VkMemoryPropertyFlags properties)
	:m_Window(window), m_Extent(extent), m_Format(format), m_Usage(usage), m_AspectFlags(aspectFlags), m_Tiling(tiling), m_Properties(properties)
{
	CreateImage();
}

Image::~Image()
{
	DestroyImage();
}

void Image::CreateImage()
{
	CreateVkImage(m_Extent, m_Format, m_Usage, m_Tiling, m_Properties, m_Image, m_ImageMem);
	m_ImageView = CreateVkImageView(m_Image, m_Format, m_AspectFlags);
}

void Image::DestroyImage()
{
	vkDestroyImageView(m_Window.GetDevice(), m_ImageView, nullptr);
	vkDestroyImage(m_Window.GetDevice(), m_Image, nullptr);
	vkFreeMemory(m_Window.GetDevice(), m_ImageMem, nullptr);
}

void Image::UpdateExtent(const Window& window)
{
	m_Extent = { window.GetSwapChainExtent().width, window.GetSwapChainExtent().height, 1 };
}

void Image::CreateVkImage(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageTiling tiling, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	m_ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	m_ImageCreateInfo.pNext = nullptr;
	m_ImageCreateInfo.flags = 0;
	m_ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	m_ImageCreateInfo.extent = extent;
	m_ImageCreateInfo.mipLevels = 1;
	m_ImageCreateInfo.arrayLayers = 1;
	m_ImageCreateInfo.format = format;
	m_ImageCreateInfo.tiling = tiling;
	m_ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	m_ImageCreateInfo.usage = usage;
	m_ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	VkResult result = vkCreateImage(m_Window.GetDevice(), &m_ImageCreateInfo, nullptr, &image);
	VkAssert(result);

	vkGetImageMemoryRequirements(m_Window.GetDevice(), image, &m_MemRequirement);

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
		std::cout << "Vulkan could not find a suitable memory for the allocation of the Image buffer!" << std::endl;

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

	result = vkAllocateMemory(m_Window.GetDevice(), &m_MemAllocInfo, nullptr, &imageMemory);
	VkAssert(result);

	result = vkBindImageMemory(m_Window.GetDevice(), image, imageMemory, 0);
	VkAssert(result);
}

VkImageView Image::CreateVkImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	m_ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	m_ImageViewCreateInfo.image = image;
	m_ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	m_ImageViewCreateInfo.format = format;
	m_ImageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	m_ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	m_ImageViewCreateInfo.subresourceRange.levelCount = 1;
	m_ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	m_ImageViewCreateInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	VkResult result = vkCreateImageView(m_Window.GetDevice(), &m_ImageViewCreateInfo, nullptr, &imageView);
	VkAssert(result);
	return imageView;
}
