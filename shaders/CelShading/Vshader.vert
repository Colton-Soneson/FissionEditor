#version 450

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	float aspectRatio;
	float screenHeight;
	float screenWidth;
	float time;
	vec3 lightSource;
	vec3 eyePos;
	float ambientStrength;
	float diffuseStrength;
	float specularStrength;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTextureCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 resolution;
layout(location = 2) out vec2 fragTextureCoord;
layout(location = 3) out vec3 fragPos;	//from OpenGL tutorial, not same as the above
layout(location = 4) out float time;

layout(location = 5) out vec3 outNormal;
layout(location = 6) out vec3 outLightSource;
layout(location = 7) out vec3 outLightVector;
layout(location = 8) out vec3 outEyePos;

layout(location = 9) out vec3 N;    
layout(location = 10) out vec3 v;	





void main() 
{
	fragTextureCoord = inTextureCoord;	
	fragPos = vec3(ubo.view * ubo.model * vec4(inPosition, 1.0));	//actual fragments position
	
	//normal matrix ( to go from model space to world space)
	mat3 normalMatrix = mat3(transpose(inverse(ubo.view * ubo.model)));
	outNormal = normalMatrix * inNormal;

	//light pos
	outLightSource = ubo.lightSource;

	//view pos
	outEyePos = ubo.eyePos;

	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
}