#include "Sync.h"
using namespace Sync;

Semaphore::Semaphore(const Window& window, int maxFramesInFlight)
	:m_Window(window), MAX_FRAMES_IN_FLIGHT(maxFramesInFlight)
{
	m_Semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	m_SemaphoreCreateInfo.pNext = nullptr;
	m_SemaphoreCreateInfo.flags = 0;

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkResult result = vkCreateSemaphore(m_Window.GetDevice(), &m_SemaphoreCreateInfo, nullptr, &m_Semaphores[i]);
		VkAssert(result);
	}
}

Semaphore::~Semaphore()
{
	for (auto& semaphore : m_Semaphores)
	{
		vkDestroySemaphore(m_Window.GetDevice(), semaphore, nullptr);
	}
}

Fence::Fence(const Window& window, int maxFramesInFlight)
	:m_Window(window), MAX_FRAMES_IN_FLIGHT(maxFramesInFlight)
{
	m_Fences.resize(MAX_FRAMES_IN_FLIGHT);
	m_FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	m_FenceCreateInfo.pNext = nullptr;
	m_FenceCreateInfo.flags = 0;

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkResult result = vkCreateFence(m_Window.GetDevice(), &m_FenceCreateInfo, nullptr, &m_Fences[i]);
		VkAssert(result);
	}
}

Fence::~Fence()
{
	for (auto& Fence : m_Fences)
	{
		vkDestroyFence(m_Window.GetDevice(), Fence, nullptr);
	}
}