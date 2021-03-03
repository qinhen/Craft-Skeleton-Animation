#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextcoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in uvec2 inBoneid;
layout (location = 5) in vec3 inBoneweight;

layout (location = 0) out vec2 outTextcoord;
layout (location = 1) out vec4 outColor;
layout (location = 2) out vec3 outNormal;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

layout(binding = 0) uniform CameraProperties 
{
	mat4 view;
	mat4 proj;
	vec4 lightPos;
} cam;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform vec4 color;


void main(void)
{
	vec4 world_space_pos = model * vec4(inPosition, 1.0);
	vec4 screen_space_pos = cam.proj * cam.view * world_space_pos;
	gl_Position = screen_space_pos;

	outTextcoord = inTextcoord;
	outColor = color.rgba;

	mat3 normal_matrix = transpose(inverse(mat3(model)));
	outNormal = normal_matrix * inNormal;
}

