#pragma once

#include <vector>
#include <memory>
#include "vulkan/vulkan.h"

#include "VertexLayout.h"
#include "DescriptorLayout.h"
#include "../../Window.h"

struct GraphicsPipelineLayout
{
private:
	const Window& m_Window;

	//CreateShader()
	std::string m_FilepathVert;
	std::vector<char> m_ShaderBinVert;
	size_t m_FileSizeVert;
	std::string m_FilepathFrag;
	std::vector<char> m_ShaderBinFrag;
	size_t m_FileSizeFrag;
	VkShaderModuleCreateInfo m_ShaderModCreateInfoVert = {};
	VkShaderModuleCreateInfo m_ShaderModCreateInfoFrag = {};
	VkShaderModule m_ShaderModVert = {};
	VkShaderModule m_ShaderModFrag = {};

	VkPipelineShaderStageCreateInfo m_ShaderStageCreateInfoVert = {};
	VkPipelineShaderStageCreateInfo m_ShaderStageCreateInfoFrag = {};
	VkPipelineVertexInputStateCreateInfo m_PipelineVertInStateCreateInfo = {};
	VkPipelineInputAssemblyStateCreateInfo m_PipelineInAssemStateCreateInfo = {};
	VkViewport m_Viewport = {};
	VkRect2D m_Scissor = {};
	VkPipelineViewportStateCreateInfo m_PipelineViewportStateCreateInfo = {};
	VkPipelineRasterizationStateCreateInfo m_PipelineRasterizerStateCreateInfo = {};
	VkPipelineMultisampleStateCreateInfo m_PipelineMultisamplingStateCreateInfo = {};
	VkPipelineDepthStencilStateCreateInfo m_PipelineDepthStencilStateCreateInfo = {};
	VkPipelineColorBlendAttachmentState m_PipelineColorBlendAttachment = {};
	VkPipelineColorBlendStateCreateInfo m_PipelineColorBlendStateCreateInfo = {};
	VkDynamicState m_DynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };
	VkPipelineDynamicStateCreateInfo m_PipelineDynamicStateCreateInfo = {};
 
public:
	GraphicsPipelineLayout(const Window& window) :m_Window(window){}
	~GraphicsPipelineLayout()
	{
		vkDestroyShaderModule(m_Window.GetDevice(), m_ShaderModVert, nullptr);
		vkDestroyShaderModule(m_Window.GetDevice(), m_ShaderModFrag, nullptr);
	}

	void VertexShader(const std::string& filePath, const char* entryPoint = "main")
	{
		m_FilepathVert = filePath;
		//Vertex Shader
		std::ifstream fileVert(m_FilepathVert, std::ios::ate | std::ios::binary);
		if (!fileVert.is_open())
		{
			std::cout << "Can not open: " << m_FilepathVert.c_str() << std::endl;
			return;
		}
		m_FileSizeVert = static_cast<size_t>(fileVert.tellg());
		m_ShaderBinVert.resize(m_FileSizeVert);
		fileVert.seekg(0);
		fileVert.read(m_ShaderBinVert.data(), m_FileSizeVert);
		fileVert.close();

		m_ShaderModCreateInfoVert.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		m_ShaderModCreateInfoVert.codeSize = m_ShaderBinVert.size();
		m_ShaderModCreateInfoVert.pCode = reinterpret_cast<const uint32_t*>(m_ShaderBinVert.data());
		m_ShaderModCreateInfoVert.pNext = nullptr;
		m_ShaderModCreateInfoVert.flags = 0;

		VkResult result = vkCreateShaderModule(m_Window.GetDevice(), &m_ShaderModCreateInfoVert, nullptr, &m_ShaderModVert);
		VkAssert(result);

		m_ShaderStageCreateInfoVert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_ShaderStageCreateInfoVert.stage = VK_SHADER_STAGE_VERTEX_BIT;
		m_ShaderStageCreateInfoVert.module = m_ShaderModVert;
		m_ShaderStageCreateInfoVert.pName = entryPoint;
		m_ShaderStageCreateInfoVert.pNext = nullptr;
		m_ShaderStageCreateInfoVert.flags = 0;
		m_ShaderStageCreateInfoVert.pSpecializationInfo = nullptr;
	}

	void FragmentShader(const std::string& filePath, const char* entryPoint = "main")
	{
		m_FilepathFrag = filePath;
		//Fragment Shader
		std::ifstream fileFrag(m_FilepathFrag, std::ios::ate | std::ios::binary);
		if (!fileFrag.is_open())
		{
			std::cout << "Can not open: " << m_FilepathFrag.c_str() << std::endl;
			return;
		}
		m_FileSizeFrag = static_cast<size_t>(fileFrag.tellg());
		m_ShaderBinFrag.resize(m_FileSizeFrag);
		fileFrag.seekg(0);
		fileFrag.read(m_ShaderBinFrag.data(), m_FileSizeFrag);
		fileFrag.close();

		m_ShaderModCreateInfoFrag.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		m_ShaderModCreateInfoFrag.codeSize = m_ShaderBinFrag.size();
		m_ShaderModCreateInfoFrag.pCode = reinterpret_cast<const uint32_t*>(m_ShaderBinFrag.data());
		m_ShaderModCreateInfoFrag.pNext = nullptr;
		m_ShaderModCreateInfoFrag.flags = 0;

		VkResult result = vkCreateShaderModule(m_Window.GetDevice(), &m_ShaderModCreateInfoFrag, nullptr, &m_ShaderModFrag);
		VkAssert(result);

		m_ShaderStageCreateInfoFrag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_ShaderStageCreateInfoFrag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		m_ShaderStageCreateInfoFrag.module = m_ShaderModFrag;
		m_ShaderStageCreateInfoFrag.pName = entryPoint;
		m_ShaderStageCreateInfoFrag.pNext = nullptr;
		m_ShaderStageCreateInfoFrag.flags = 0;
		m_ShaderStageCreateInfoFrag.pSpecializationInfo = nullptr;
	}

	void VertexInput(const VertexLayout& m_VertexShaderLayout)
	{
		m_PipelineVertInStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		m_PipelineVertInStateCreateInfo.pNext = nullptr;
		m_PipelineVertInStateCreateInfo.flags = 0;
		m_PipelineVertInStateCreateInfo.vertexBindingDescriptionCount = m_VertexShaderLayout.vertexBindingDescriptionCount; //Describe the VBO structure
		m_PipelineVertInStateCreateInfo.pVertexBindingDescriptions = m_VertexShaderLayout.pVertexBindingDescriptions;
		m_PipelineVertInStateCreateInfo.vertexAttributeDescriptionCount = m_VertexShaderLayout.vertexAttributeDescriptionCount;
		m_PipelineVertInStateCreateInfo.pVertexAttributeDescriptions = m_VertexShaderLayout.pVertexAttributeDescriptions;
	}

	void InputAssembly(VkPrimitiveTopology topology)
	{
		m_PipelineInAssemStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		m_PipelineInAssemStateCreateInfo.pNext = nullptr;
		m_PipelineInAssemStateCreateInfo.flags = 0;
		m_PipelineInAssemStateCreateInfo.topology = topology; //Style of drawing
		m_PipelineInAssemStateCreateInfo.primitiveRestartEnable = VK_FALSE;
	}

	void Viewport()
	{
		m_Viewport.x = 0.0f;
		m_Viewport.y = 0.0f;
		m_Viewport.width = static_cast<float>(m_Window.GetSwapChainExtent().width);
		m_Viewport.height = static_cast<float>(m_Window.GetSwapChainExtent().height);
		m_Viewport.minDepth = 0.0f;
		m_Viewport.maxDepth = 1.0f;
		m_Scissor.offset = { 0, 0 };
		m_Scissor.extent = m_Window.GetSwapChainExtent();

		m_PipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		m_PipelineViewportStateCreateInfo.viewportCount = 1;
		m_PipelineViewportStateCreateInfo.pViewports = &m_Viewport;
		m_PipelineViewportStateCreateInfo.scissorCount = 1;
		m_PipelineViewportStateCreateInfo.pScissors = &m_Scissor;
	}

	void Rasterizer(bool depthClamp = VK_FALSE, bool rasterizerDiscard = VK_FALSE, VkPolygonMode polygon = VK_POLYGON_MODE_FILL, VkCullModeFlagBits cull = VK_CULL_MODE_BACK_BIT, VkFrontFace frontface = VK_FRONT_FACE_COUNTER_CLOCKWISE)
	{
		m_PipelineRasterizerStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_PipelineRasterizerStateCreateInfo.depthClampEnable = depthClamp; //Forces all geometry into the depth field
		m_PipelineRasterizerStateCreateInfo.rasterizerDiscardEnable = rasterizerDiscard; //Culls out of bound geometry
		m_PipelineRasterizerStateCreateInfo.polygonMode = polygon; //Fill type
		m_PipelineRasterizerStateCreateInfo.lineWidth = 1.0f;
		m_PipelineRasterizerStateCreateInfo.cullMode = cull;
		m_PipelineRasterizerStateCreateInfo.frontFace = frontface; //Specifies the vertex order for faces to be considered front-facing !!!CHECK IBO SO THAT TRIANGLE ARE IN ORDER, ELSE THEY WILL NOT RENDER!!!
		m_PipelineRasterizerStateCreateInfo.depthBiasEnable = VK_FALSE;
		m_PipelineRasterizerStateCreateInfo.depthBiasConstantFactor = 0.0f;
		m_PipelineRasterizerStateCreateInfo.depthBiasClamp = 0.0f;
		m_PipelineRasterizerStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	}

	void Multisampling(int numSamples = 1)
	{
		m_PipelineMultisamplingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		m_PipelineMultisamplingStateCreateInfo.sampleShadingEnable = numSamples > 1 ? VK_TRUE : VK_FALSE; //Currently disabled
		m_PipelineMultisamplingStateCreateInfo.rasterizationSamples = (VkSampleCountFlagBits)numSamples;
		m_PipelineMultisamplingStateCreateInfo.minSampleShading = 1.0f;
		m_PipelineMultisamplingStateCreateInfo.pSampleMask = nullptr;
		m_PipelineMultisamplingStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		m_PipelineMultisamplingStateCreateInfo.alphaToOneEnable = VK_FALSE;
	}

	void DepthStenciling()
	{
		m_PipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		m_PipelineDepthStencilStateCreateInfo.pNext = nullptr;
		m_PipelineDepthStencilStateCreateInfo.flags = 0;
		m_PipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		m_PipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		m_PipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS; //Fragment discards
		m_PipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE; //Ranged based testing
		m_PipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE; // Stencil testing
		m_PipelineDepthStencilStateCreateInfo.front = {};
		m_PipelineDepthStencilStateCreateInfo.back = {};
		m_PipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
		m_PipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;
	}

	void ColourBlending()
	{
		m_PipelineColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		m_PipelineColorBlendAttachment.blendEnable = VK_TRUE; //Alpha blending
		m_PipelineColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_PipelineColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		m_PipelineColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		m_PipelineColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		m_PipelineColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		m_PipelineColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		m_PipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		m_PipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		m_PipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		m_PipelineColorBlendStateCreateInfo.attachmentCount = 1;
		m_PipelineColorBlendStateCreateInfo.pAttachments = &m_PipelineColorBlendAttachment;
		m_PipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		m_PipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		m_PipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		m_PipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;
	}

	void DynamicState()
	{
		m_PipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		m_PipelineDynamicStateCreateInfo.pNext = nullptr;
		m_PipelineDynamicStateCreateInfo.flags = 0;
		m_PipelineDynamicStateCreateInfo.dynamicStateCount = 2;
		m_PipelineDynamicStateCreateInfo.pDynamicStates = m_DynamicStates;
	}

	const VkPipelineShaderStageCreateInfo* GetShaderStageCreateInfoVertPtr()const { return &m_ShaderStageCreateInfoVert; }
	const VkPipelineShaderStageCreateInfo* GetShaderStageCreateInfoFragPtr()const { return &m_ShaderStageCreateInfoFrag; }
	const VkPipelineVertexInputStateCreateInfo* GetPipelineVertInStateCreateInfoPtr()const { return &m_PipelineVertInStateCreateInfo; }
	const VkPipelineInputAssemblyStateCreateInfo* GetPipelineInAssemStateCreateInfoPtr()const { return &m_PipelineInAssemStateCreateInfo; }
	const VkPipelineViewportStateCreateInfo* GetPipelineViewportStateCreateInfoPtr()const { return &m_PipelineViewportStateCreateInfo; }
	const VkPipelineRasterizationStateCreateInfo* GetPipelineRasterizerStateCreateInfoPtr()const { return &m_PipelineRasterizerStateCreateInfo; }
	const VkPipelineMultisampleStateCreateInfo* GetPipelineMultisamplingStateCreateInfoPtr()const { return &m_PipelineMultisamplingStateCreateInfo; }
	const VkPipelineDepthStencilStateCreateInfo* GetPipelineDepthStencilStateCreateInfoPtr()const{ return &m_PipelineDepthStencilStateCreateInfo; }
	const VkPipelineColorBlendStateCreateInfo* GetPipelineColorBlendStateCreateInfoPtr()const { return &m_PipelineColorBlendStateCreateInfo; }

	void UpdateViewport(const Window& window)
	{
		m_Viewport.x = 0.0f;
		m_Viewport.y = 0.0f;
		m_Viewport.width = static_cast<float>(window.GetSwapChainExtent().width);
		m_Viewport.height = static_cast<float>(window.GetSwapChainExtent().height);
		m_Viewport.minDepth = 0.0f;
		m_Viewport.maxDepth = 1.0f;
		m_Scissor.offset = { 0, 0 };
		m_Scissor.extent = window.GetSwapChainExtent();

		m_PipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		m_PipelineViewportStateCreateInfo.viewportCount = 1;
		m_PipelineViewportStateCreateInfo.pViewports = &m_Viewport;
		m_PipelineViewportStateCreateInfo.scissorCount = 1;
		m_PipelineViewportStateCreateInfo.pScissors = &m_Scissor;
	}
};