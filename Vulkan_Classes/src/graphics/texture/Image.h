#pragma once
#include <vector>
#include "vulkan/vulkan.h"
#include "../Window.h"

class Image
{
private:
	const Window& m_Window;

	VkExtent3D m_Extent; 
	VkFormat m_Format; 
	VkImageUsageFlags m_Usage; 
	VkImageAspectFlags m_AspectFlags; 
	VkImageTiling m_Tiling;
	VkMemoryPropertyFlags m_Properties;

	VkImage m_Image;
	VkImageCreateInfo m_ImageCreateInfo = {};

	VkDeviceMemory m_ImageMem;
	VkMemoryRequirements m_MemRequirement;
	VkMemoryAllocateInfo m_MemAllocInfo;
	
	VkImageView m_ImageView;
	VkImageViewCreateInfo m_ImageViewCreateInfo = {};

public:
	Image(const Window& window, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectFlags, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	~Image();

	void CreateImage();
	void DestroyImage();

	void UpdateExtent(const Window& window);

	inline VkImage GetImage() const { return m_Image; }
	inline VkImageView GetImageView() const { return m_ImageView; }
	
private:
	void CreateVkImage(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageTiling tiling, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView CreateVkImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

};

