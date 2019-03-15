#pragma once
#include <vector>
#include "vulkan/vulkan.h"
#include "../Window.h"

namespace Cmd {
class Pool
{
private:
	const Window& m_Window;
	std::vector<VkCommandPool> m_CmdPools;
	std::vector<VkCommandPoolCreateInfo> m_CmdPoolsCreateInfo;

public:
	Pool(const Window& window);
	~Pool();

	inline const Window& GetWindow() const { return m_Window; }
	inline const std::vector<VkCommandPool> GetCmdPool() const { return m_CmdPools; }
};
};