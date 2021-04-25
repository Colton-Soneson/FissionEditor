#pragma once
#include "DeviceSelection.h"

struct UniformBufferObject
{
	//oh here we go again...
	//they say use align as with this stuff because nested for loops can ruin the contiguous mem

	//alignas(16) glm::mat4 model;
	//alignas(16) glm::mat4 view;
	//alignas(16) glm::mat4 proj;

	//if you GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;

	//my stuff
	glm::float32 aspectRatio;
	glm::float32 screenHeight;
	glm::float32 screenWidth;
	glm::float32 time;
	glm::float32 activeLight;
	glm::float32 ambientMod;
	glm::float32 diffuseMod;
	glm::float32 specularMod;
	glm::float32 lightIntensity;
	glm::vec3 lightSource;
	glm::vec3 eyePos;

};

