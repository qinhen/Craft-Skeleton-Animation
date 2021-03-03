#version 450

layout (location = 0) in vec2 inPosition;

layout (location = 0) out vec4 outColor;

layout(binding = 0) uniform SkyInfo
{
	mat4 inverseVpMatrix;
	vec4 viewPos;
	vec4 direction;
	vec4 params[10];
	vec2 inverseDepthFactor;
} sky;


vec3 HosekWilkie(float cos_theta, float gamma, float cos_gamma) {
	vec3 A = sky.params[0].xyz;
	vec3 B = sky.params[1].xyz;
	vec3 C = sky.params[2].xyz;
	vec3 D = sky.params[3].xyz;
	vec3 E = sky.params[4].xyz;
	vec3 F = sky.params[5].xyz;
	vec3 G = sky.params[6].xyz;
	vec3 H = sky.params[7].xyz;
	vec3 I = sky.params[8].xyz;
	vec3 Z = sky.params[9].xyz;
	vec3 chi = (1 + cos_gamma * cos_gamma) / pow(1 + H * H - 2 * cos_gamma * H, vec3(1.5));
	return (1 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * sqrt(cos_theta));
}


void main() 
{
	
	float inverseLinearDepth = sky.inverseDepthFactor.x + sky.inverseDepthFactor.y * 1.0;
	float linearDepth = 1.0 / inverseLinearDepth;
	vec4 screenSpacePos = vec4(inPosition, 1.0, 1.0);
	vec4 projectionPos = linearDepth * screenSpacePos;
	vec3 position = (projectionPos * sky.inverseVpMatrix).xyz;
	
	//position.y = (position.y + sky.viewPos.y) / (1.0 + sky.viewPos.y);
	vec3 direction = normalize(position);
	direction.y = (direction.y + 0.05) / 1.05;
	direction = normalize(direction);


	//if(direction.y < 0)
	//{
	//	direction = sky.direction.xyz;
	//	direction.y = 0;
	//	direction = normalize(direction);
	//}
	vec3 skyLightDir = sky.direction.xyz;
	skyLightDir.y = (skyLightDir.y + 0.05) / 1.05;
	skyLightDir = normalize(skyLightDir);

	float cos_theta = clamp(direction.y, 0, 1);
	float cos_gamma = dot(direction, skyLightDir);
	float gamma = acos(cos_gamma);

	vec3 Z = sky.params[9].xyz;
	vec3 R = Z * HosekWilkie(cos_theta, gamma, cos_gamma);
	if (cos_gamma > 0 && sky.direction.y > -0.05) 
		R = R + vec3(pow(cos_gamma, 2048) * 0.5);


	//if (direction.y < 0)
	//	if (direction.y > -0.05)
	//		R = mix(R, vec3(0.4), smoothstep(0,1,-direction.y / 0.05));
	//	else
	//		R = vec3(0.4);

	outColor = vec4(R, 1.0);
	//outColor = vec4(1.0);
	outColor = vec4(R, 1.0) ;
}

