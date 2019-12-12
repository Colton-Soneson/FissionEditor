#version 440

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
	mat4 lightSpaceMatrix;
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


layout(location = 11) out float outAmbientStrength;
layout(location = 12) out float outDiffuseStrength;
layout(location = 13) out float outSpecularStrength;

layout(location = 14) out vec4 fragPosLightSpace;


void main() 
{
	//lighting / shading
	outAmbientStrength = ubo.ambientStrength;
	outDiffuseStrength = ubo.diffuseStrength;
	outSpecularStrength = ubo.specularStrength;
	
	fragTextureCoord = inTextureCoord;	

	fragPos = vec3((ubo.view * ubo.model) * vec4(inPosition, 1.0));	//actual fragments position
	
	fragPosLightSpace = ubo.lightSpaceMatrix * vec4(fragPos, 1.0);

	//normal matrix ( to go from model space to world space)
	//mat3 normalMatrix = mat3(transpose(inverse(ubo.model))); 
	mat3 normalMatrix = mat3(ubo.view * ubo.model); 
	outNormal = normalMatrix * inNormal;
	//outNormal = inNormal;
	//light pos
	outLightSource = ubo.lightSource;
	//outLightSource = ubo.lightSource;

	//view pos
	outEyePos = ubo.eyePos;

	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
}