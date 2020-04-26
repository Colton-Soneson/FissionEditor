#pragma once
#include "DeviceSelection.h"


////manual glfw, will automatically load the vulkan.h alongside it
//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>
//#include <vulkan/vulkan.h>
//
////GLM linear alg stuff
//#define GLM_FORCE_RADIANS	//makes it so shit like rotate uses radians instead of eulerAngles
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES		//This is for solving contiguous memory for you (may have problems with nested stuff)
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE		//depth testing, configure matrix to range of 0 to 1 instead of -1 to 1
//#define GLM_ENABLE_EXPERIMENTAL			//to use with hash functions
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>		//rotate, lookAt, perspective
//#include <glm/gtx/hash.hpp>
//
////error reporting
//#include <iostream>
//#include <stdexcept>
//
////lambda functions / resource management
//#include <functional>
//
////macros for exit sucess and failure
//#include <cstdlib>
//
////math / extras
//#include <assert.h>	//need this for NDEBUG i think
//#include <vector>
//#include <map>
//#include <cstring>
//#include <set>
//#include <optional>	//a wrapper that has no value until you assign something to it
//#include <cstdint>  //needed for UINT32_MAX
//#include <algorithm>	//max and min
//#include <fstream>		//shader loading
//#include <array>
//#include <chrono>		//use for time, this can determine math is the same regaurdless of framerate
//#include <unordered_map>	//use for vertex duplication on 3d models
//
//#include <iostream>

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

