#pragma once
#include "vulkan/vulkan.h"
#include "CmdPool.h"

namespace Cmd {
class SingleTimeCommand
{
public:
	static VkCommandBuffer Begin(const Cmd::Pool& cmdPool)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = cmdPool.GetCmdPool()[0]; //Default use first;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer cmdBuffer;
		vkAllocateCommandBuffers(cmdPool.GetWindow().GetDevice(), &allocInfo, &cmdBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(cmdBuffer, &beginInfo);

		return cmdBuffer;
	}

	static void End(const Cmd::Pool& cmdPool, VkCommandBuffer cmdBuffer)
	{
		vkEndCommandBuffer(cmdBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		vkQueueSubmit(cmdPool.GetWindow().GetQueues()[0], 1, &submitInfo, VK_NULL_HANDLE); //Default use first;
		vkQueueWaitIdle(cmdPool.GetWindow().GetQueues()[0]);

		vkFreeCommandBuffers(cmdPool.GetWindow().GetDevice(), cmdPool.GetCmdPool()[0], 1, &cmdBuffer);
	}
};
}