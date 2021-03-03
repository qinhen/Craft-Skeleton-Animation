#version 450

layout (location = 0) in vec2 inTextcoord;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

layout(binding = 0) uniform CameraProperties 
{
	mat4 view;
	mat4 proj;
	vec4 lightPos;
} cam;

void main() 
{
	vec3 lightVector = normalize(cam.lightPos.xyz);
	float dot_product = max(dot(lightVector, normalize(inNormal)), 0.6);
	vec3 color = inColor.rgb * vec3(dot_product);
	outColor = vec4(color, inColor.a);
}