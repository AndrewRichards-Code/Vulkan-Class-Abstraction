#version 450
//From Application
layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 colours;
layout(location = 2) in vec2 texCoord;

layout(binding = 0) uniform UniformBufferObject
{ 
	mat4 proj; 
	mat4 view; 
	mat4 modl;
} ubo;

//To Fragment Shader
layout(location = 0) out vec3 v_Colour;
layout(location = 1) out vec2 v_TexCoord;

void main()
{
	gl_Position = ubo.proj * ubo.view * ubo.modl * vec4(positions, 1.0);
	v_Colour = colours;
	v_TexCoord = texCoord;
}