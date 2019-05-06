#pragma once
#include <iostream>
#include <functional>
#include "vulkan/vulkan.h"
#include "Window.h"
#include "cmd/CmdBuffer.h"
#include "Sync.h"

class Renderer
{private:
	std::vector<Cmd::Buffer*> m_CmdBuffers;
	Window& m_Window;
	Shader& m_Shader;
	Image& m_Depth;
	Framebuffer& m_Framebuffer; 
	
	uint32_t m_ImageIndex;
	VkSubmitInfo m_SubmitInfo;
	VkPresentInfoKHR m_PresentInfo;

public:
	bool m_ResubmitCommandBuffers = true;

public:
	Renderer(Window& window, Shader& shader, Image& depth, Framebuffer& framebuffer);

	void Submit(Cmd::Buffer* cmdbuffer);
	void DrawFrame(const Sync::Fence& inFlight, const Sync::Semaphore& imageAvailable, const Sync::Semaphore& renderFinished, int currentFrame, int imageIndex);
	void RecreateSwapChain();

};
