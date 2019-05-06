#include "graphics/Window.h"
#include "graphics/shader/Shader.h"
#include "graphics/texture/Image.h"
#include "graphics/texture/Texture.h"
#include "graphics/buffer/Framebuffer.h"
#include "graphics/buffer/VertexBuffer.h"
#include "graphics/buffer/IndexBuffer.h"
#include "graphics/buffer/UniformBuffer.h"
#include "graphics/shader/inputs/DescriptorPoolSets.h"
#include "graphics/Sync.h"
#include "graphics/cmd/CmdBuffer.h"
#include "graphics/Renderer.h"

#include <chrono>

int main()
{
	Window window(1920, 1080);
	Cmd::Pool cmdPool(window);

	struct VertexData
	{
		glm::vec3 position;
		glm::vec3 colour;
		glm::vec2 texCoord;
	};
	VertexLayout vertex(sizeof(VertexData));
	vertex.AddAttribute(0, 0, VK_VEC3, offsetof(VertexData, position));
	vertex.AddAttribute(0, 1, VK_VEC3, offsetof(VertexData, colour));
	vertex.AddAttribute(0, 2, VK_VEC2, offsetof(VertexData, texCoord));
	RenderPassLayout renderPass;
	renderPass.AddAttachment(0, { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }, window.GetSwapChainImageFormat(), VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
	renderPass.AddAttachment(1, { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }, Shader::StaticFindDepthFormat(window), VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);
	renderPass.AddSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS, -1, 0, -1, 1, -1);
	renderPass.AddSubpassDependency(VK_SUBPASS_EXTERNAL, 0, { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT });
	Descriptor::Layout descriptor;
	descriptor.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	descriptor.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	GraphicsPipelineLayout gplayout(window);
	gplayout.VertexShader("res/shaders/vk_basic.vert.spv");
	gplayout.FragmentShader("res/shaders/vk_basic.frag.spv");
	gplayout.VertexInput(vertex);
	gplayout.InputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	gplayout.Viewport();
	gplayout.Rasterizer();
	gplayout.Multisampling();
	gplayout.DepthStenciling();
	gplayout.ColourBlending();
	gplayout.DynamicState();
	Shader shader(renderPass, descriptor, gplayout, window);

	Image depthImage(window, { window.GetSwapChainExtent().width, window.GetSwapChainExtent().height, 1 }, Shader::StaticFindDepthFormat(window), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	Framebuffer framebuffer(window, shader, window.GetSwapChainImageViews(), depthImage.GetImageView());

	Texture texture1("res/img/andrew_manga_3_square.png", cmdPool);
	Texture texture2("res/img/GEAR_logo_square.png", cmdPool);

	std::vector<VertexData> vertices = {
	{ { -0.5f, -0.5f, +0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } },
	{ { +0.5f, -0.5f, +0.5f },{ 1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f } },
	{ { +0.5f, +0.5f, +0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
	{ { -0.5f, +0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },

	{ { -0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
	{ { +0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
	{ { +0.5f, +0.5f, -0.5f },{ 0.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
	{ { -0.5f, +0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } }
	};

	std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0,
		1, 5, 6, 6, 2, 1,
		5, 4, 7, 7, 6, 5,
		4, 0, 3, 3, 7, 4,
		3, 2, 6, 6, 7, 3,
		1, 0, 4, 4, 5, 1
	};

	struct CameraUBO
	{
		glm::mat4 proj;
		glm::mat4 view;
		glm::mat4 modl;
	};
	CameraUBO CameraUBO;
	CameraUBO.proj = glm::perspective(glm::radians(90.0f), window.GetRatio(), 0.1f, 10.0f);
	CameraUBO.proj[1][1] *= -1; //Flips proj for Vulkan NDC;
	CameraUBO.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	VertexBuffer VBO(window, cmdPool, sizeof(VertexData) * vertices.size(), vertices.data());
	IndexBuffer IBO(window, cmdPool, sizeof(uint32_t) * indices.size(), indices.data());
	UniformBuffer UBO(window, sizeof(CameraUBO));

	Descriptor::Pool descPool(window, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER });
	Descriptor::Sets descSets1(window, descPool, shader);
	descSets1.AddUniformBuffer(UBO, 0);
	descSets1.AddTexture(texture1, 1);
	descSets1.Update();

	Renderer renderer(window, shader, depthImage, framebuffer);
	Cmd::Buffer cmdBuffer(cmdPool);

	int MAX_FRAMES_IN_FLIGHT = 2;
	Sync::Semaphore imageAvailable(window, MAX_FRAMES_IN_FLIGHT);
	Sync::Semaphore renderFinished(window, MAX_FRAMES_IN_FLIGHT);
	Sync::Fence inFlight(window, MAX_FRAMES_IN_FLIGHT);

	bool INITIAL_SUBMIT = true;
	int IMAGE_INDEX = 0;
	int CURRENT_FRAME = 0;
	while (!glfwWindowShouldClose(window.GetGLFWwindow()))
	{
		window.Update();

		//Update UBO
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = 0.5f * std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		CameraUBO.proj = glm::perspective(glm::radians(90.0f), window.GetRatio(), 0.1f, 10.0f);
		CameraUBO.proj[1][1] *= -1; //Flips proj for Vulkan NDC;
		CameraUBO.modl = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, -2.0f)) * glm::rotate(glm::mat4(1.0f), deltaTime * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//Initial and Re- Submission of CmdBuffers
		if (renderer.m_ResubmitCommandBuffers)
		{
			if (!INITIAL_SUBMIT)
			{
				cmdBuffer.DestroyCommandbuffer();
				cmdBuffer.CreateCommandbuffer();
			}

			cmdBuffer.SetClearValues({ 0.5, 0.5, 0.5, 1.0 });
			cmdBuffer.Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
			cmdBuffer.BeginRenderPass(shader, framebuffer);
			cmdBuffer.BindPipeline(shader);
			cmdBuffer.BindVertexBuffer(VBO);
			cmdBuffer.BindIndexBuffer(IBO);
			cmdBuffer.BindBindDescriptorSets(shader, descSets1);
			cmdBuffer.Draw(IBO);
			cmdBuffer.EndRenderPass();
			cmdBuffer.End();

			renderer.Submit(&cmdBuffer);
			INITIAL_SUBMIT = false;
			renderer.m_ResubmitCommandBuffers = false;
		}
		//Renderering
		renderer.DrawFrame(inFlight, imageAvailable, renderFinished, CURRENT_FRAME, IMAGE_INDEX);
		UBO.UpdateUniformBuffers(&CameraUBO, IMAGE_INDEX);
		
		//Update Frame number
		CURRENT_FRAME = (CURRENT_FRAME + 1) % inFlight.MaxFramesInFlight();
	}
	vkDeviceWaitIdle(window.GetDevice());
}


