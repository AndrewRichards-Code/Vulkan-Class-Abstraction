#include "CmdBuffer.h"

using namespace Cmd;

Buffer::Buffer(const Pool& cmdPool)
	:m_CmdPool(cmdPool)
{
	CreateCommandbuffer();

	m_ClearColours[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_ClearColours[1].depthStencil = { 1.0f, 0 };
}

Buffer::~Buffer()
{
	DestroyCommandbuffer();
}

void Buffer::CreateCommandbuffer()
{
	size_t swapChainSize = m_CmdPool.GetWindow().GetSwapChainImageViews().size();
	m_CmdBuffers.resize(swapChainSize);
	m_CmdBufferAllocInfos.reserve(swapChainSize);
	for (int i = 0; i < static_cast<int32_t>(swapChainSize); i++)
	{
		VkCommandBufferAllocateInfo temp = {};
		temp.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		temp.commandBufferCount = 1;
		temp.commandPool = m_CmdPool.GetCmdPool()[0]; //Uses the CmdPools from PhysicalDevice[0] and QueueFamily[0]
		temp.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		m_CmdBufferAllocInfos.push_back(temp);

		VkResult result = vkAllocateCommandBuffers(m_CmdPool.GetWindow().GetDevice(), &temp, &m_CmdBuffers[i]);
		VkAssert(result);
	}
}

void Buffer::DestroyCommandbuffer()
{
	vkFreeCommandBuffers(m_CmdPool.GetWindow().GetDevice(), m_CmdPool.GetCmdPool()[0], static_cast<uint32_t>(m_CmdBuffers.size()), m_CmdBuffers.data()); //Uses the CmdPools from PhysicalDevice[0] and QueueFamily[0]
	m_CmdBufferAllocInfos.clear();
	m_CmdBuffers.clear();
}

void Buffer::Begin(VkCommandBufferUsageFlagBits usage)
{
	m_CmdBufferBeginInfos.reserve(m_CmdBuffers.size());
	for (int i = 0; i < static_cast<int32_t>(m_CmdBuffers.size()); i++)
	{
		VkCommandBufferBeginInfo temp = {};
		temp.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		temp.flags = usage;
		temp.pInheritanceInfo = nullptr;
		m_CmdBufferBeginInfos.push_back(temp);

		VkResult result = vkBeginCommandBuffer(m_CmdBuffers[i], &temp);
		VkAssert(result);
	}
}

void Buffer::End()
{
	for (auto cmdBuffer : m_CmdBuffers)
	{
		vkEndCommandBuffer(cmdBuffer);
	}
	m_CmdBufferBeginInfos.clear();
}

void Buffer::BeginRenderPass(const Shader& shader, const Framebuffer& framebuffer)
{

	for (int i = 0; i < static_cast<int32_t>(m_CmdBuffers.size()); i++)
	{
		VkRenderPassBeginInfo temp = {};
		temp.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		temp.pNext = nullptr;
		temp.renderPass = shader.GetRenderPass();
		temp.framebuffer = framebuffer.GetFramebuffers()[i];
		temp.renderArea.offset = { 0, 0 };
		temp.renderArea.extent = m_CmdPool.GetWindow().GetSwapChainExtent();
		temp.clearValueCount = static_cast<uint32_t>(m_ClearColours.size());
		temp.pClearValues = m_ClearColours.data();
		m_RenderPassBeginInfos.push_back(temp);

		vkCmdBeginRenderPass(m_CmdBuffers[i], &temp, VK_SUBPASS_CONTENTS_INLINE);
	}
}

void Buffer::EndRenderPass()
{
	for (auto cmdBuffer : m_CmdBuffers)
	{
		vkCmdEndRenderPass(cmdBuffer);
	}
	m_RenderPassBeginInfos.clear();
}

void Buffer::BindPipeline(const Shader& shader)
{
	for (auto cmdBuffer : m_CmdBuffers)
	{
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.GetPipeline());
	}
}
void Buffer::BindVertexBuffer(const VertexBuffer& vbo)
{
	for (auto cmdBuffer : m_CmdBuffers)
	{
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vbo.GetBuffer(), m_OffsetVert);
	}
}
void Buffer::BindIndexBuffer(const IndexBuffer& ibo)
{
	for (auto cmdBuffer : m_CmdBuffers)
	{
		vkCmdBindIndexBuffer(cmdBuffer, ibo.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}
}
void Buffer::BindBindDescriptorSets(const Shader& shader, const Descriptor::Sets& sets)
{
	for (int i = 0; i < static_cast<int>(m_CmdBuffers.size()); i++) 
	{
		vkCmdBindDescriptorSets(m_CmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, shader.GetPipelineLayout(), 0, 1, &sets.GetDescriptorSets()[i], 0, nullptr);
	}
}
void Buffer::Draw(const IndexBuffer& ibo)
{
	for (auto cmdBuffer : m_CmdBuffers)
	{
		vkCmdDrawIndexed(cmdBuffer, (ibo.GetCount() / sizeof(uint32_t)), 1, 0, 0, 0);
	}
}
