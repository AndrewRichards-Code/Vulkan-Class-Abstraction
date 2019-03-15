#include "HelloTriangle.h"

HelloTriangle::HelloTriangle(int width, int height)
{
	SetupVertexBuffer();
	m_VulkanContext = std::make_unique<Vulkan>(width, height, m_VertexData, m_IndexData);
}


HelloTriangle::~HelloTriangle()
{

}

void HelloTriangle::Run()
{
	m_VulkanContext->BeignCommandbufferRecord();
	m_VulkanContext->BeginRenderPass();
	m_VulkanContext->EndRenderPass();
	m_VulkanContext->EndCommandbufferRecord();

	while (!glfwWindowShouldClose(m_VulkanContext->m_Window))
	{
		m_VulkanContext->Update();
		DrawFrame();
	}
	vkDeviceWaitIdle(m_VulkanContext->m_Device);
}

void HelloTriangle::DrawFrame()
{
	VkResult result = vkWaitForFences(m_VulkanContext->m_Device, 1, &m_VulkanContext->m_InFlightFences[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	if (result != VK_SUCCESS)
		std::cout << "Vulkan: Error with vkWaitForFences! Error: " << result << std::endl;
	
	result = vkAcquireNextImageKHR(m_VulkanContext->m_Device, m_VulkanContext->m_SwapChain, std::numeric_limits<uint64_t>::max(), m_VulkanContext->m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_ImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		m_VulkanContext->RecreateSwapChain(); 
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		std::cout << "Vulkan was unable to acquire next image!" << std::endl;
	
	VkSemaphore waitSemaphores[1] = { m_VulkanContext->m_ImageAvailableSemaphores[m_CurrentFrame] };
	VkSemaphore signalSemaphores[1] = { m_VulkanContext->m_RenderFinishedSemaphores[m_CurrentFrame] };
	VkPipelineStageFlags waitStages[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	m_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	m_SubmitInfo.pNext = nullptr;
	m_SubmitInfo.waitSemaphoreCount = 1;
	m_SubmitInfo.pWaitSemaphores = waitSemaphores;
	m_SubmitInfo.signalSemaphoreCount = 1;
	m_SubmitInfo.pSignalSemaphores = signalSemaphores;
	m_SubmitInfo.pWaitDstStageMask = waitStages;
	m_SubmitInfo.commandBufferCount = 1;
	m_SubmitInfo.pCommandBuffers = &m_VulkanContext->m_CmdBuffers[m_ImageIndex];
	
	result = vkResetFences(m_VulkanContext->m_Device, 1, &m_VulkanContext->m_InFlightFences[m_CurrentFrame]);
	if (result != VK_SUCCESS)
		std::cout << "Vulkan: Error with vkResetFences!" << std::endl;

	UpdateUBO(m_ImageIndex);

	result = vkQueueSubmit(m_VulkanContext->m_Queues[0], 1, &m_SubmitInfo, m_VulkanContext->m_InFlightFences[m_CurrentFrame]); //Use PhysicalDevice[0] and QueueFamily[0]
	if (result != VK_SUCCESS)
		std::cout << "Vulkan was unable to submit the Command Buffer!" << std::endl;

	VkSwapchainKHR swapChains[1] = { m_VulkanContext->m_SwapChain };

	m_PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	m_PresentInfo.pNext = nullptr;
	m_PresentInfo.waitSemaphoreCount = 1;
	m_PresentInfo.pWaitSemaphores = signalSemaphores;
	m_PresentInfo.swapchainCount = 1;
	m_PresentInfo.pSwapchains = swapChains;
	m_PresentInfo.pImageIndices = &m_ImageIndex;
	m_PresentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(m_VulkanContext->m_Queues[0], &m_PresentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_VulkanContext->m_FramebufferResized)
	{	
		m_VulkanContext->m_FramebufferResized = false;
		m_VulkanContext->RecreateSwapChain();
	}
	else if (result != VK_SUCCESS)
		std::cout << "Vulkan was unable to present the queue!" << std::endl;
	
	m_CurrentFrame = (m_CurrentFrame + 1) % m_VulkanContext->MAX_FRAMES_IN_FLIGHT;
}

void HelloTriangle::UpdateUBO(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float deltaTime = 0.5f * std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	
	int glfwExtentWidth, glfwExtentHeight;
	glfwGetFramebufferSize(m_VulkanContext->m_Window, &glfwExtentWidth, &glfwExtentHeight);
	float aspectRatio = (float)glfwExtentWidth / (float)glfwExtentHeight;

	m_UBO.proj = glm::perspective(glm::radians(90.0f), aspectRatio, 0.1f, 10.0f);
	m_UBO.view = glm::lookAt(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_UBO.modl = glm::rotate(glm::mat4(1.0f), deltaTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_UBO.proj[1][1] *= -1; //Flips proj for Vulkan NDC;
	m_UBO.currentImage = currentImage;

	m_VulkanContext->m_UBO = m_UBO;
	m_VulkanContext->UpdateUniformBuffers();
}

void HelloTriangle::SetupVertexBuffer() //Call before initialising Vulkan;
{
	m_Vertices = {
	{ { -0.5f, -0.5f, +0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } },
	{ { +0.5f, -0.5f, +0.5f },{ 1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f } },
	{ { +0.5f, +0.5f, +0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
	{ { -0.5f, +0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },

	{ { -0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
	{ { +0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
	{ { +0.5f, +0.5f, -0.5f },{ 0.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
	{ { -0.5f, +0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } }
	};

	m_Indices = {
		0, 1, 2, 2, 3, 0,
		1, 5, 6, 6, 2, 1,
		5, 4, 7, 7, 6, 5,
		4, 0, 3, 3, 7, 4,
		3, 2, 6, 6, 7, 3,
		1, 0, 4, 4, 5, 1
	};
	
	m_VertexBindingDesc.binding = 0; //VBO Index
	m_VertexBindingDesc.stride = sizeof(VertexData);
	m_VertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	m_VertexAttribDesc.resize(3);
	m_VertexAttribDesc[0].binding = 0; //VBO Index
	m_VertexAttribDesc[0].location = 0; //location in vertex shader
	m_VertexAttribDesc[0].format = VK_VEC3;
	m_VertexAttribDesc[0].offset = offsetof(VertexData, position);

	m_VertexAttribDesc[1].binding = 0; //VBO Index
	m_VertexAttribDesc[1].location = 1; //location in vertex shader
	m_VertexAttribDesc[1].format = VK_VEC3;
	m_VertexAttribDesc[1].offset = offsetof(VertexData, colour);

	m_VertexAttribDesc[2].binding = 0; //VBO Index
	m_VertexAttribDesc[2].location = 2; //location in vertex shader
	m_VertexAttribDesc[2].format = VK_VEC2;
	m_VertexAttribDesc[2].offset = offsetof(VertexData, texCoord);

	m_VertexData.vertexBindingDescriptionCount = 1;
	m_VertexData.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexAttribDesc.size());
	m_VertexData.pVertexBindingDescriptions = &m_VertexBindingDesc;
	m_VertexData.pVertexAttributeDescriptions = m_VertexAttribDesc.data();
	m_VertexData.verticesSize = sizeof(m_Vertices[0]) * m_Vertices.size();
	m_VertexData.verticesCount = static_cast<uint32_t>(m_Vertices.size());
	m_VertexData.pVertexData = reinterpret_cast<float*>(m_Vertices.data());

	m_IndexData.verticesSize = sizeof(m_Indices[0]) * m_Indices.size();
	m_IndexData.verticesCount = static_cast<uint32_t>(m_Indices.size());
	m_IndexData.pIndexData = reinterpret_cast<uint16_t*>(m_Indices.data());
}