#version 440

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	float aspectRatio;
	float screenHeight;
	float screenWidth;
	float time;
	float activeLight;
	float ambientMod;
	float diffuseMod;
	float specularMod;
	float lightIntensity;
	vec3 lightSource;
	vec3 eyePos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTextureCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec2 resolution;
layout(location = 1) out vec2 fragTextureCoord;
layout(location = 2) out float time;

layout(location = 3) out vec3 vLightSource;
layout(location = 4) out float vLightIntensity;
layout(location = 5) out vec3 vEyePos;

layout(location = 6) out vec4 vMV_nrm_by_inNorm;
layout(location = 7) out vec4 vMV_pos;

layout(location = 8) out float vAmbientMod;
layout(location = 9) out float vDiffuseMod;
layout(location = 10) out float vSpeculartMod;

layout(location = 11) out float vActiveLight;

//when it comes to morph targets on gpu, you have multiple keyframes 

//one morphingModel and X keyframes per, they actually come from a source file that come from X number of meshes (with same topology) exported
//		from the obj file. You then interpolate between all these values.

//maybe pass in a UBO with all the keyframe data

//during morph, the 2d texture coords do not get morphed (they still fit their same points on the model)


////this is dans way 
//struct sMorphTarget
//{
//	vec4 mPosition, mNormal, mTangent;
//};
//
//layout (location = 0) in sMorphTarget aMorphTarget[5];


//no attribute for bitangent?
//vec4 aBiTangent = vec4(cross(aNormal.xyz, aTangent.xyz), 0.0)

//vTangentBasisView = uMV_nrm * mat4(aTangent, aBiTangent, aNormal, vec4(0.0))

//vtangentbasisview[3] = uMV * aPosition
//gl_position 


//float t = float(uTime * 0.1) //this is just adjustable playback speed
//float u = fract(t)
//int indexForCurrentKeyframe = int(t) % [NUMBER OF KEYFRAMES]
//int indexForNextKeyframe = (indexForCurrentKeyframe + 1) % [NUMBER OF KEYFRAMES]
//int iN = (indexForNextKeyframe + 1) % [NUMBER OF KEYFRAMES]
//int iP = (indexForCurrentKeyframe + REMAINING NUMBER OF KEYFRAMES if 5 total then 4) % [NUMBER OF KEYFRAMES]

//sMorphTarget k;
//sMorphTarget k0  =  aMorphTarget[indexForCurrentKeyframe];	//the number represents what keyframe we are on, maybe in vulkan version we pass in this data in UBO
//sMorphTarget k1  =  aMorphTarget[indexForNextKeyframe];

//step function [OPTION ONE]
//k = k0;

//nearest [OPTION TWO]
//k = u < 0.5 ? k0 : k1;

//lerp [OPTION THREE]  just make this a function
//k.mPosition = lerp(k0.mPosition, k1.mPosition, u)	//dan defined lerp (look at utilCommon)
//k.mNormal = nlerp(k0.mNormal, k1.mNormal, u)		//dan defined nlerp
//k.mTangent = nlerp(k0.mTangent, k1.mTangent, u)

//Catmull Rom [OPTION FOUR]
//vec4 CatmullRom(in vec4 vP, in vec4 v0, in vec4 v1, in vec4 vN, in float u) {};	//dan defined this for us
//vec4 CatmullRom(in vec4 vP, in vec4 v0, in vec4 v1, in vec4 vN, in float u, ) {};	//make another function for this based on one above
//go into dans for this stuff, look at the back half of the class video


//vec4 aPosition = k.mPosition;
//vec4 aNormal = k.mNormal;
//vec4 aTangent = k.mTangent;


void main() 
{	
	mat4 MV_nrm = inverse(transpose((ubo.view * ubo.model)));

	vMV_nrm_by_inNorm = MV_nrm * vec4(inNormal, 1.0);
	vMV_pos = (ubo.view * ubo.model) * vec4(inPosition, 1.0);

	vEyePos = ubo.eyePos;
	vLightSource = vec3(ubo.view * vec4(ubo.lightSource, 1.0));
	fragTextureCoord = inTextureCoord;

	vLightIntensity = ubo.lightIntensity;

	vAmbientMod = ubo.ambientMod;

	vActiveLight = ubo.activeLight;

	gl_Position = ubo.proj * vMV_pos;
}