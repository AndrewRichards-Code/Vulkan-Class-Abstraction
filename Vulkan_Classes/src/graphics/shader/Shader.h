#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "../Debug.h"
#include "../Window.h"
#include "inputs/VertexLayout.h"
#include "inputs/RenderPassLayout.h"
#include "inputs/DescriptorLayout.h"
#include "inputs/GraphicsPipelineLayout.h"

class Shader
{
private:
	const RenderPassLayout& m_RenderPassLayout;
	const Descriptor::Layout& m_DescriptorLayout;
	GraphicsPipelineLayout& m_GraphicsPipelineLayout;
	const Window& m_Window;

	//CreateRenderPass() 
	VkRenderPassCreateInfo m_RenderPassCreateInfo = {};
	VkRenderPass m_RenderPass;

	//CreateDescriptorLayout()
	VkDescriptorSetLayoutCreateInfo m_DescSetLayoutCreateInfo = {};
	VkDescriptorSetLayout m_DescSetLayout = {};

	//CreateGraphicsPipeline()
	VkPipelineLayoutCreateInfo m_PipelineLayoutCreateInfo = {};
	VkPipelineLayout m_PipelineLayout;
	VkGraphicsPipelineCreateInfo m_GraphicPipelineCreateInfo = {};
	VkPipeline m_Pipeline;

public:
	Shader(const RenderPassLayout& renderPassLayout, const Descriptor::Layout& descriptorLayout, GraphicsPipelineLayout& graphicsPipelinelayout,const Window& window);
	~Shader();

	void CreateRenderPass(); 
	void DestroyRenderPass();

	void CreateGraphicPipeline();
	void DestroyGraphicPipeline();

private:
	void CreateDescriptorLayout();
	void DestroyDescriptorLayout();

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat FindDepthFormat();

public:
	static VkFormat StaticFindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, const Window& m_Window);
	static VkFormat StaticFindDepthFormat(const Window& window);

	inline const VkRenderPassCreateInfo& GetRenderPassCreateInfo() const { return m_RenderPassCreateInfo; }
	inline const VkRenderPass& GetRenderPass() const { return m_RenderPass; }
	inline const VkPipelineLayout& GetPipelineLayout() const { return m_PipelineLayout; }
	inline const VkPipeline& GetPipeline() const { return m_Pipeline; }

	inline VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescSetLayout; }
};
