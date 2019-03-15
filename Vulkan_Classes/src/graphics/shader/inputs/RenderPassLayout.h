#pragma once

#include <vector>
#include <array>
#include "vulkan/vulkan.h"

struct RenderPassLayout
{
private:
	std::vector<VkAttachmentDescription> m_AttachmentsDesc;
	std::vector<VkAttachmentReference> m_AttachmentRef;

	std::vector<VkSubpassDescription> m_SubpassDescriptions;
	std::vector<VkSubpassDependency> m_SubpassDependencies;

public:
	//RenderPassLayout() {};

	//layout[0] = AttachmentDesc.initialLayout 
	//layout[1] = AttachmentDesc.finalLayout
	//layout[2] = AttachmentRef.layout 
	void AddAttachment(int index, std::array<VkImageLayout, 3> layout, VkFormat format, VkSampleCountFlagBits samples,
		VkAttachmentLoadOp loadOps, VkAttachmentStoreOp storeOps, VkAttachmentLoadOp stencilLoadOps = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentStoreOp stencilStoreOps = VK_ATTACHMENT_STORE_OP_DONT_CARE)
	{
		VkAttachmentDescription tempDesc = {};
		tempDesc.format = format;
		tempDesc.samples = samples;
		tempDesc.loadOp = loadOps;
		tempDesc.storeOp = storeOps;
		tempDesc.stencilLoadOp = stencilLoadOps;
		tempDesc.stencilStoreOp = stencilStoreOps;
		tempDesc.initialLayout = layout[0];
		tempDesc.finalLayout = layout[1];
		m_AttachmentsDesc.push_back(tempDesc);

		VkAttachmentReference tempRef = {};
		tempRef.attachment = index;
		tempRef.layout = layout[2];
		m_AttachmentRef.push_back(tempRef);
	}

	void AddSubpass(VkPipelineBindPoint bindPoint, 
		int inputAttachments = -1,
		int colourAttachments = -1,
		int resolveAttachments = -1,
		int depthStencilAttachments = -1,
		int preserveAttachments = -1
		)
	{
		VkSubpassDescription temp = {};
		temp.flags;
		temp.pipelineBindPoint = bindPoint;
		temp.inputAttachmentCount = inputAttachments == -1 ? 0 : 1;
		temp.pInputAttachments = inputAttachments == -1 ? nullptr : &m_AttachmentRef[inputAttachments];
		temp.colorAttachmentCount = colourAttachments == -1 ? 0 : 1;
		temp.pColorAttachments = colourAttachments == -1 ? nullptr : &m_AttachmentRef[colourAttachments];
		temp.pResolveAttachments = resolveAttachments == -1 ? nullptr : &m_AttachmentRef[resolveAttachments];
		temp.pDepthStencilAttachment = depthStencilAttachments == -1 ? nullptr : &m_AttachmentRef[depthStencilAttachments];
		temp.preserveAttachmentCount = preserveAttachments == -1 ? 0 : 1;
		temp.pPreserveAttachments = preserveAttachments == -1 ? nullptr : (uint32_t*)&m_AttachmentRef[preserveAttachments];

		m_SubpassDescriptions.push_back(temp);
	}

	//masks[0] = Dependency.srcStageMask; 
	//masks[1] = Dependency.srcAccessMask;
	//masks[2] = Dependency.dstStageMask; 
	//masks[3] = Dependency.dstAccessMask;
	void AddSubpassDependency(int srcSubpass, int dstSubpass, std::array<int, 4> masks)
	{
		VkSubpassDependency temp = {};
		temp.srcSubpass = srcSubpass;
		temp.dstSubpass = dstSubpass;
		temp.srcStageMask = masks[0];
		temp.srcAccessMask = masks[1];
		temp.dstStageMask = masks[2];
		temp.dstAccessMask = masks[3];
		temp.dependencyFlags;
		
		m_SubpassDependencies.push_back(temp);
	}

	inline const std::vector<VkAttachmentDescription> GetAttachmentDescriptions() const { return m_AttachmentsDesc; }
	inline const std::vector<VkSubpassDescription> GetSubpassDescriptions() const { return m_SubpassDescriptions; }
	inline const std::vector<VkSubpassDependency> GetSubpassDependencies() const { return m_SubpassDependencies; }
	
	inline const VkAttachmentDescription* const GetAttachmentDescriptionsPtr() const { return m_AttachmentsDesc.data(); }
	inline const VkSubpassDescription* const GetSubpassDescriptionsPtr() const { return m_SubpassDescriptions.data(); }
	inline const VkSubpassDependency* const GetSubpassDependenciesPtr() const { return m_SubpassDependencies.data(); }
};