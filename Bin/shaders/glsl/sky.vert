#version 450 core

layout (location = 0) in vec2 inPosition;
layout (location = 0) out vec2 outPosition;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

layout(binding = 0) uniform SkyInfo
{
	mat4 inverseVpMatrix;
	vec4 viewPos;
	vec4 direction;
	vec4 params[10];
	vec2 inverseDepthFactor;
} sky;



void main(void)
{
	outPosition = inPosition;
	gl_Position = vec4(inPosition, 1.0, 1.0);
}

