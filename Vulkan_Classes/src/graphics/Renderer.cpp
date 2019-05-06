#include "Renderer.h"

Renderer::Renderer( Window& window, Shader& shader, Image& depth, Framebuffer& framebuffer)
	: m_Window(window), m_Shader(shader), m_Depth(depth), m_Framebuffer(framebuffer)
{
}

void Renderer::Submit(Cmd::Buffer* cmdbuffer)
{
	m_CmdBuffers.push_back(cmdbuffer);
}

void Renderer::DrawFrame(const Sync::Fence& inFlight, const Sync::Semaphore& imageAvailable, const Sync::Semaphore& renderFinished, int currentFrame, int imageIndex)
{
	VkResult result = vkWaitForFences(m_Window.GetDevice(), 1, &inFlight.GetFences()[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	if (result != VK_SUCCESS)
		std::cout << "Vulkan: Error with vkWaitForFences! Error: " << result << std::endl;

	result = vkAcquireNextImageKHR(m_Window.GetDevice(), m_Window.GetSwapChain(), std::numeric_limits<uint64_t>::max(), imageAvailable.GetSemaphores()[currentFrame], VK_NULL_HANDLE, &m_ImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		std::cout << "Vulkan was unable to acquire next image!" << std::endl;

	imageIndex = m_ImageIndex;

	VkSemaphore waitSemaphores[1] = { imageAvailable.GetSemaphores()[currentFrame] };
	VkSemaphore signalSemaphores[1] = { renderFinished.GetSemaphores()[currentFrame] };
	VkPipelineStageFlags waitStages[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	std::vector<VkCommandBuffer> submitCmdBuffers;
	submitCmdBuffers.resize(m_CmdBuffers.size());
	for (size_t i = 0; i < m_CmdBuffers.size(); i++)
		submitCmdBuffers[i] = m_CmdBuffers[i]->GetCmdBuffers()[m_ImageIndex];

	m_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	m_SubmitInfo.pNext = nullptr;
	m_SubmitInfo.waitSemaphoreCount = 1;
	m_SubmitInfo.pWaitSemaphores = waitSemaphores;
	m_SubmitInfo.signalSemaphoreCount = 1;
	m_SubmitInfo.pSignalSemaphores = signalSemaphores;
	m_SubmitInfo.pWaitDstStageMask = waitStages;
	m_SubmitInfo.commandBufferCount = static_cast<uint32_t>(m_CmdBuffers.size());
	m_SubmitInfo.pCommandBuffers = submitCmdBuffers.data();

	result = vkResetFences(m_Window.GetDevice(), 1, &inFlight.GetFences()[currentFrame]);
	if (result != VK_SUCCESS)
		std::cout << "Vulkan: Error with vkResetFences!" << std::endl;

	result = vkQueueSubmit(m_Window.GetQueues()[0], 1, &m_SubmitInfo, inFlight.GetFences()[currentFrame]); //Use PhysicalDevice[0] and QueueFamily[0]
	if (result != VK_SUCCESS)
		std::cout << "Vulkan was unable to submit the Command Buffer!" << std::endl;

	VkSwapchainKHR swapChains[1] = { m_Window.GetSwapChain() };

	m_PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	m_PresentInfo.pNext = nullptr;
	m_PresentInfo.waitSemaphoreCount = 1;
	m_PresentInfo.pWaitSemaphores = signalSemaphores;
	m_PresentInfo.swapchainCount = 1;
	m_PresentInfo.pSwapchains = swapChains;
	m_PresentInfo.pImageIndices = &m_ImageIndex;
	m_PresentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(m_Window.GetQueues()[0], &m_PresentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.m_FramebufferResized)
	{
		m_Window.m_FramebufferResized = false;
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS)
		std::cout << "Vulkan was unable to present the queue!" << std::endl;
}

void Renderer::RecreateSwapChain()
{
	//Check if window minimised
	int glfwExtentWidth = 0, glfwExtentHeight = 0;
	while (glfwExtentWidth == 0 || glfwExtentHeight == 0)
	{
		glfwGetFramebufferSize(m_Window.GetGLFWwindow(), &glfwExtentWidth, &glfwExtentHeight);
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(m_Window.GetDevice());

	//Recreate the SwapChain and Dependent structures
	m_CmdBuffers.clear();
	
	m_Framebuffer.DestroyFramebuffer();
	m_Depth.DestroyImage();
	m_Shader.DestroyGraphicPipeline();
	m_Shader.DestroyRenderPass();
	m_Window.DestroySwapChainImageViews();
	m_Window.DestroySwapChain();

	m_Window.CreateSwapChain();
	m_Window.CreateSwapChainImageViews();
	m_Shader.CreateRenderPass();
	m_Shader.CreateGraphicPipeline();
	m_Depth.UpdateExtent(m_Window);
	m_Depth.CreateImage();
	m_Framebuffer.UpdateImageView(m_Window.GetSwapChainImageViews(), m_Depth.GetImageView());
	m_Framebuffer.CreateFramebuffer();
	
	m_ResubmitCommandBuffers = true;
}

