#pragma once

#include "vulkan/vulkan.h"
#include "Window.h"


namespace Sync
{
class Semaphore
{
private:
	const Window& m_Window;
	int MAX_FRAMES_IN_FLIGHT;
	std::vector<VkSemaphore> m_Semaphores;
	VkSemaphoreCreateInfo m_SemaphoreCreateInfo;

public:
	Semaphore(const Window& window, int maxFramesInFlight);
	~Semaphore();

	inline const std::vector<VkSemaphore>& GetSemaphores() const { return m_Semaphores; }
	inline const int MaxFramesInFlight() const { return MAX_FRAMES_IN_FLIGHT; }
};

class Fence
{
private:
	const Window& m_Window;
	int MAX_FRAMES_IN_FLIGHT;
	std::vector<VkFence> m_Fences;
	VkFenceCreateInfo m_FenceCreateInfo;

public:
	Fence(const Window& window, int maxFramesInFlight);
	~Fence();

	inline const std::vector<VkFence>& GetFences() const { return m_Fences; }
	inline const int MaxFramesInFlight() const { return MAX_FRAMES_IN_FLIGHT; }
};
}