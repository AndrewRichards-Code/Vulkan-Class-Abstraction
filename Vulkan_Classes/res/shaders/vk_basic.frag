#version 450
//To Post-Processing
layout(location = 0) out vec4 colour;

//From Vertex Shader
layout(location = 0) in vec3 v_Colour;
layout(location = 1) in vec2 v_TexCoord;

//From Application
layout(binding = 1) uniform sampler2D texSampler;

void main()
{
	colour = texture(texSampler, v_TexCoord);
}