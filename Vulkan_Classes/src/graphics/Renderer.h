#pragma once
#include <iostream>
#include <functional>
#include "vulkan/vulkan.h"
#include "Window.h"
#include "cmd/CmdBuffer.h"
#include "Sync.h"

class Renderer
{private:
	Cmd::Buffer& m_CmdBuffer;
	Window& m_Window;
	Shader& m_Shader;
	Image& m_Depth;
	Framebuffer& m_Framebuffer; 
	VertexBuffer& m_VBO; 
	IndexBuffer& m_IBO; 
	Descriptor::Sets& m_DescSets;
	
	uint32_t m_ImageIndex;
	VkSubmitInfo m_SubmitInfo;
	VkPresentInfoKHR m_PresentInfo;

public:
	Renderer(Cmd::Buffer& cmdbuffers, Window& window, Shader& shader, Image& depth, Framebuffer& framebuffer, VertexBuffer& VBO, IndexBuffer& IBO, Descriptor::Sets& descSets);

	void DrawFrame(const Sync::Fence& inFlight, const Sync::Semaphore& imageAvailable, const Sync::Semaphore& renderFinished, int currentFrame);
	void RecreateSwapChain();
};
