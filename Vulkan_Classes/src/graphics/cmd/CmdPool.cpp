#include "CmdPool.h"

using namespace Cmd;

Pool::Pool(const Window& window)
	:m_Window(window)
{
	size_t size = m_Window.GetQueueFamilyProperties().size();
	m_CmdPools.resize(size);
	m_CmdPoolsCreateInfo.reserve(size);
	for (int i = 0; i < static_cast<int32_t>(size); i++)
	{
		VkCommandPoolCreateInfo temp = {};
		temp.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		temp.pNext = nullptr;
		temp.queueFamilyIndex = m_Window.GetDeviceQueueCreateInfo()[i].queueFamilyIndex;
		temp.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		m_CmdPoolsCreateInfo.push_back(temp);

		VkResult result = vkCreateCommandPool(m_Window.GetDevice(), &temp, nullptr, &m_CmdPools[i]);
		VkAssert(result);
	}
}

Pool::~Pool()
{
	for (auto cmdPool : m_CmdPools)
	{
		vkDestroyCommandPool(m_Window.GetDevice(), cmdPool, nullptr);
	}
}