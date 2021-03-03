#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextcoord;
layout (location = 3) in vec2 inTextcoord2;
layout (location = 4) in vec3 inTangent;
layout (location = 5) in uvec3 inBones;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

layout(binding = 0) uniform CameraProperties 
{
	mat4 view;
	mat4 proj;
	vec4 viewDir;
	vec4 viewPos;
	vec4 lightPos;
	vec4 lightColor;
	vec4 lightAmbient;
} cam;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform vec4 color;

void main(void)
{
	vec4 world_space_pos = model * vec4(inPosition, 1.0);
	vec4 screen_space_pos = cam.proj * cam.view * world_space_pos;
	gl_Position = screen_space_pos;
}

