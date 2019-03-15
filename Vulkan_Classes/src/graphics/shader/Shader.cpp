#include "Shader.h"

Shader::Shader( 
	const RenderPassLayout& renderPassLayout, 
	const Descriptor::Layout& descriptorLayout, 
	GraphicsPipelineLayout& graphicsPipelinelayout,
	const Window& window) 
	:m_RenderPassLayout(renderPassLayout), 
	m_DescriptorLayout(descriptorLayout), 
	m_GraphicsPipelineLayout(graphicsPipelinelayout),
	m_Window(window)
{
	CreateRenderPass();
	CreateDescriptorLayout();
	CreateGraphicPipeline();
}

Shader::~Shader()
{
	DestroyGraphicPipeline();
	DestroyDescriptorLayout();
	DestroyRenderPass();
}

void Shader::CreateRenderPass()
{
	m_RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	m_RenderPassCreateInfo.pNext = nullptr;
	m_RenderPassCreateInfo.flags = 0;
	m_RenderPassCreateInfo.attachmentCount = (uint32_t)m_RenderPassLayout.GetAttachmentDescriptions().size();
	m_RenderPassCreateInfo.pAttachments = m_RenderPassLayout.GetAttachmentDescriptionsPtr();
	m_RenderPassCreateInfo.subpassCount = (uint32_t)m_RenderPassLayout.GetSubpassDescriptions().size();
	m_RenderPassCreateInfo.pSubpasses = m_RenderPassLayout.GetSubpassDescriptionsPtr();
	m_RenderPassCreateInfo.dependencyCount = (uint32_t)m_RenderPassLayout.GetSubpassDependencies().size();
	m_RenderPassCreateInfo.pDependencies = m_RenderPassLayout.GetSubpassDependenciesPtr();

	VkResult result = vkCreateRenderPass(m_Window.GetDevice(), &m_RenderPassCreateInfo, nullptr, &m_RenderPass);
	VkAssert(result);
}

void Shader::DestroyRenderPass()
{
	vkDestroyRenderPass(m_Window.GetDevice(), m_RenderPass, nullptr);
}

void Shader::CreateDescriptorLayout()
{
	m_DescSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	m_DescSetLayoutCreateInfo.pNext = nullptr;
	m_DescSetLayoutCreateInfo.flags = 0;
	m_DescSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(m_DescriptorLayout.GetDescriptorSetLayoutBindings().size()); //Number of VkDescriptorSetLayoutBinding
	m_DescSetLayoutCreateInfo.pBindings = m_DescriptorLayout.GetDescriptorSetLayoutBindingsPtr();

	VkResult result = vkCreateDescriptorSetLayout(m_Window.GetDevice(), &m_DescSetLayoutCreateInfo, nullptr, &m_DescSetLayout);
	VkAssert(result);

}

void Shader::DestroyDescriptorLayout()
{
	vkDestroyDescriptorSetLayout(m_Window.GetDevice(), m_DescSetLayout, nullptr);
}

void Shader::CreateGraphicPipeline()
{
	if (m_GraphicsPipelineLayout.GetPipelineViewportStateCreateInfoPtr()->pViewports->width != m_Window.GetSwapChainExtent().width ||
		m_GraphicsPipelineLayout.GetPipelineViewportStateCreateInfoPtr()->pViewports->height != m_Window.GetSwapChainExtent().height)
	{
		m_GraphicsPipelineLayout.UpdateViewport(m_Window);
	}
	VkPipelineShaderStageCreateInfo shaderStages[2] = { *m_GraphicsPipelineLayout.GetShaderStageCreateInfoVertPtr(), *m_GraphicsPipelineLayout.GetShaderStageCreateInfoFragPtr() };

	//Pipeline Layout
	m_PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	m_PipelineLayoutCreateInfo.pNext = nullptr;
	m_PipelineLayoutCreateInfo.flags = 0;
	m_PipelineLayoutCreateInfo.setLayoutCount = 1; //For the UBO
	m_PipelineLayoutCreateInfo.pSetLayouts = &m_DescSetLayout;
	m_PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	m_PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	VkResult result = vkCreatePipelineLayout(m_Window.GetDevice(), &m_PipelineLayoutCreateInfo, nullptr, &m_PipelineLayout);
	VkAssert(result);

	m_GraphicPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	m_GraphicPipelineCreateInfo.pNext = nullptr;
	m_GraphicPipelineCreateInfo.flags = 0;
	m_GraphicPipelineCreateInfo.stageCount = 2;
	m_GraphicPipelineCreateInfo.pStages = shaderStages;
	m_GraphicPipelineCreateInfo.pVertexInputState = m_GraphicsPipelineLayout.GetPipelineVertInStateCreateInfoPtr();
	m_GraphicPipelineCreateInfo.pInputAssemblyState = m_GraphicsPipelineLayout.GetPipelineInAssemStateCreateInfoPtr();
	m_GraphicPipelineCreateInfo.pViewportState = m_GraphicsPipelineLayout.GetPipelineViewportStateCreateInfoPtr();
	m_GraphicPipelineCreateInfo.pRasterizationState = m_GraphicsPipelineLayout.GetPipelineRasterizerStateCreateInfoPtr();
	m_GraphicPipelineCreateInfo.pMultisampleState = m_GraphicsPipelineLayout.GetPipelineMultisamplingStateCreateInfoPtr();
	m_GraphicPipelineCreateInfo.pDepthStencilState = m_GraphicsPipelineLayout.GetPipelineDepthStencilStateCreateInfoPtr();
	m_GraphicPipelineCreateInfo.pColorBlendState = m_GraphicsPipelineLayout.GetPipelineColorBlendStateCreateInfoPtr();
	m_GraphicPipelineCreateInfo.pDynamicState = nullptr;
	m_GraphicPipelineCreateInfo.layout = m_PipelineLayout;
	m_GraphicPipelineCreateInfo.renderPass = m_RenderPass;
	m_GraphicPipelineCreateInfo.subpass = 0;
	m_GraphicPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	m_GraphicPipelineCreateInfo.basePipelineIndex = -1;

	result = vkCreateGraphicsPipelines(m_Window.GetDevice(), VK_NULL_HANDLE, 1, &m_GraphicPipelineCreateInfo, nullptr, &m_Pipeline);
	VkAssert(result);
}

void Shader::DestroyGraphicPipeline()
{
	vkDestroyPipeline(m_Window.GetDevice(), m_Pipeline, nullptr);
	vkDestroyPipelineLayout(m_Window.GetDevice(), m_PipelineLayout, nullptr);
}

VkFormat Shader::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(m_Window.GetPhysicalDevices()[0], format, &formatProps);

		if (tiling == VK_IMAGE_TILING_LINEAR && (formatProps.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (formatProps.optimalTilingFeatures & features) == features)
		{
			return format;
		}
		else
		{
			std::cout << "Vulkan failed to find a supported format!" << std::endl;
			return format;
		}
	}
	return VkFormat{ VK_FORMAT_UNDEFINED };
}

VkFormat Shader::FindDepthFormat()
{
	return FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat Shader::StaticFindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, const Window& m_Window)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(m_Window.GetPhysicalDevices()[0], format, &formatProps);

		if (tiling == VK_IMAGE_TILING_LINEAR && (formatProps.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (formatProps.optimalTilingFeatures & features) == features)
		{
			return format;
		}
		else
		{
			std::cout << "Vulkan failed to find a supported format!" << std::endl;
			return format;
		}
	}
	return VkFormat{ VK_FORMAT_UNDEFINED };
}

VkFormat Shader::StaticFindDepthFormat(const Window& window)
{
	return StaticFindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, window);
}