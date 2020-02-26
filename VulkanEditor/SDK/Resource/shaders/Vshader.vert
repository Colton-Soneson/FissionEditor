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

} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTextureCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 resolution;
layout(location = 2) out vec2 fragTextureCoord;
layout(location = 3) out float time;

layout(location = 4) out vec3 vNormal;
layout(location = 5) out vec3 vLightSource;
layout(location = 6) out vec3 vLightVector;
layout(location = 7) out vec3 vEyePos;

layout(location = 8) out vec4 vMV_nrm_by_inNorm;
layout(location = 9) out vec4 vMV_pos;


void main() 
{	

//	fragTextureCoord = inTextureCoord;	
//
//	fragPos = vec3((ubo.view * ubo.model) * vec4(inPosition, 1.0));	//actual fragments position
//	
//	fragPosLightSpace = ubo.lightSpaceMatrix * vec4(fragPos, 1.0);
//
//	//normal matrix ( to go from model space to world space)
//	//mat3 normalMatrix = mat3(transpose(inverse(ubo.model))); 
//	mat3 normalMatrix = mat3(ubo.view * ubo.model); 
//	outNormal = normalMatrix * inNormal;
//	//outNormal = inNormal;
//	//light pos
//	outLightSource = ubo.lightSource;
//	//outLightSource = ubo.lightSource;
//
//	//view pos
//	outEyePos = ubo.eyePos;
//	

	mat4 MV_nrm = inverse(transpose((ubo.view * ubo.model)));

	vMV_nrm_by_inNorm = MV_nrm * vec4(inNormal, 1.0);
	vMV_pos = (ubo.view * ubo.model) * vec4(inPosition, 1.0);

	vEyePos = ubo.eyePos;
	vLightSource = vec3(ubo.view * vec4(ubo.lightSource, 1.0));
	fragTextureCoord = inTextureCoord;


	gl_Position = ubo.proj * vMV_pos;
}