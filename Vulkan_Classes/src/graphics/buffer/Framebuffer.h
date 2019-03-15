#pragma once
#include "vulkan/vulkan.h"

#include <vector>

#include "../Window.h"
#include "../shader/Shader.h"

class Framebuffer 
{
private:
	const Window& m_Window;
	const Shader& m_Shader;

	std::vector<VkImageView> m_SwapChainAttachments; 
	VkImageView m_DepthAttachment;

	std::vector<VkFramebuffer> m_Framebuffers;
	std::vector<VkFramebufferCreateInfo> m_FramebufferCreateInfos;
	
public:
	Framebuffer(const Window& window, const Shader& shader, const std::vector<VkImageView>& swapChainAttachments, const VkImageView& depthAttachment);
	~Framebuffer();

	void CreateFramebuffer();
	void DestroyFramebuffer();

	void UpdateImageView(const std::vector<VkImageView>& swapChainAttachments, const VkImageView& depthAttachment);

	inline const std::vector<VkFramebuffer>& GetFramebuffers() const { return m_Framebuffers; }
};
