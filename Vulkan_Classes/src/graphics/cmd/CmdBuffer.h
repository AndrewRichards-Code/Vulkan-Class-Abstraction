#pragma once

#include "vulkan/vulkan.h"
#include "CmdPool.h"
#include "../shader/Shader.h"
#include "../buffer/Framebuffer.h"
#include "../buffer/VertexBuffer.h"
#include "../buffer/IndexBuffer.h"
#include "../shader/inputs/DescriptorPoolSets.h"

namespace Cmd
{
class Buffer
{
private:
	const Cmd::Pool& m_CmdPool;

	std::vector<VkCommandBuffer> m_CmdBuffers;
	std::vector<VkCommandBufferAllocateInfo> m_CmdBufferAllocInfos;
	std::vector<VkCommandBufferBeginInfo> m_CmdBufferBeginInfos;

	//BeginRenderPass()
	std::vector<VkRenderPassBeginInfo> m_RenderPassBeginInfos;
	std::array<VkClearValue, 2> m_ClearColours;
	VkDeviceSize m_OffsetVert[1] = { 0 };

public:
	Buffer(const Pool& cmdPool);
	~Buffer();

	void CreateCommandbuffer();
	void DestroyCommandbuffer();

	void Begin(VkCommandBufferUsageFlagBits usage);
	void End();

	void BeginRenderPass(const Shader& shader, const Framebuffer& framebuffer);
	void EndRenderPass();

	void BindPipeline(const Shader& shader);
	void BindVertexBuffer(const VertexBuffer& vbo);
	void BindIndexBuffer(const IndexBuffer& ibo);
	void BindBindDescriptorSets(const Shader& shader, const Descriptor::Sets& sets);
	void Draw(const IndexBuffer& ibo);

	inline const std::vector<VkCommandBuffer>& GetCmdBuffers() const { return m_CmdBuffers; }
};
}
