#include "Renderer.h"

Renderer::Renderer(Cmd::Buffer& cmdbuffers, Window& window, Shader& shader, Image& depth, Framebuffer& framebuffer, VertexBuffer& VBO, IndexBuffer& IBO, Descriptor::Sets& descSets)
	:m_CmdBuffer(cmdbuffers), m_Window(window), m_Shader(shader), m_Depth(depth), m_Framebuffer(framebuffer), m_VBO(VBO), m_IBO(IBO), m_DescSets(descSets)
{
}

void Renderer::DrawFrame(const Sync::Fence& inFlight, const Sync::Semaphore& imageAvailable, const Sync::Semaphore& renderFinished, int currentFrame)
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

	VkSemaphore waitSemaphores[1] = { imageAvailable.GetSemaphores()[currentFrame] };
	VkSemaphore signalSemaphores[1] = { renderFinished.GetSemaphores()[currentFrame] };
	VkPipelineStageFlags waitStages[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	m_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	m_SubmitInfo.pNext = nullptr;
	m_SubmitInfo.waitSemaphoreCount = 1;
	m_SubmitInfo.pWaitSemaphores = waitSemaphores;
	m_SubmitInfo.signalSemaphoreCount = 1;
	m_SubmitInfo.pSignalSemaphores = signalSemaphores;
	m_SubmitInfo.pWaitDstStageMask = waitStages;
	m_SubmitInfo.commandBufferCount = 1;
	m_SubmitInfo.pCommandBuffers = &m_CmdBuffer.GetCmdBuffers()[m_ImageIndex];

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
	m_CmdBuffer.DestroyCommandbuffer();
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
	m_CmdBuffer.CreateCommandbuffer();

	//Resubmitted the CmdBuffer and RenderPass
	m_CmdBuffer.Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
	m_CmdBuffer.BeginRenderPass(m_Shader, m_Framebuffer);
	m_CmdBuffer.BindPipeline(m_Shader);
	m_CmdBuffer.BindVertexBuffer(m_VBO);
	m_CmdBuffer.BindIndexBuffer(m_IBO);
	m_CmdBuffer.BindBindDescriptorSets(m_Shader, m_DescSets);
	m_CmdBuffer.Draw(m_IBO);
	m_CmdBuffer.EndRenderPass();
	m_CmdBuffer.End();
}

