#include "Framebuffer.h"

Framebuffer::Framebuffer(const Window& window, const Shader& shader, const std::vector<VkImageView>& swapChainAttachments, const VkImageView& depthAttachment)
	:m_Window(window), m_Shader(shader), m_SwapChainAttachments(swapChainAttachments), m_DepthAttachment(depthAttachment)
{
	CreateFramebuffer();
}

Framebuffer::~Framebuffer()
{
	DestroyFramebuffer();
}

void Framebuffer::CreateFramebuffer()
{
	size_t size = m_SwapChainAttachments.size();
	m_Framebuffers.resize(size);
	m_FramebufferCreateInfos.reserve(size);
	for (int i = 0; i < static_cast<int32_t>(size); i++)
	{
		std::vector<VkImageView> attachments = { m_SwapChainAttachments[i], m_DepthAttachment };
		ASSERT(attachments.size() == m_Shader.GetRenderPassCreateInfo().attachmentCount)

			VkFramebufferCreateInfo temp = {};
		temp.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		temp.pNext = nullptr;
		temp.flags = 0;
		temp.renderPass = m_Shader.GetRenderPass();
		temp.attachmentCount = static_cast<uint32_t>(attachments.size());
		temp.pAttachments = attachments.data();
		temp.width = m_Window.GetSwapChainExtent().width;
		temp.height = m_Window.GetSwapChainExtent().height;
		temp.layers = 1;
		m_FramebufferCreateInfos.push_back(temp);

		VkResult result = vkCreateFramebuffer(m_Window.GetDevice(), &temp, nullptr, &m_Framebuffers[i]);
		VkAssert(result);
	}
}

void Framebuffer::DestroyFramebuffer()
{
	for (auto framebuffer : m_Framebuffers)
	{
		vkDestroyFramebuffer(m_Window.GetDevice(), framebuffer, nullptr);
	}
	m_FramebufferCreateInfos.clear();
	m_Framebuffers.clear();
}

void Framebuffer::UpdateImageView(const std::vector<VkImageView>& swapChainAttachments, const VkImageView& depthAttachment)
{
	if (m_Framebuffers.empty())
	{
		m_SwapChainAttachments.clear();
		m_SwapChainAttachments.resize(swapChainAttachments.size());
		for(size_t i = 0; i < m_SwapChainAttachments.size(); i++)
		{
			m_SwapChainAttachments[i] = swapChainAttachments[i];
		}
		m_DepthAttachment = depthAttachment;
	}
	else
	{
		std::cout << "You must call DestroyFramebuffer(), before calling UpdateImageView()" << std::endl;
	}
}
