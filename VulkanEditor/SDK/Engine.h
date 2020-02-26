#pragma once

//ImGUI running on vulkan and glfw
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_glfw.h>
#include <ImGUI//imgui_impl_vulkan.h>


//manual glfw, will automatically load the vulkan.h alongside it
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

//GLM linear alg stuff
#define GLM_FORCE_RADIANS	//makes it so shit like rotate uses radians instead of eulerAngles
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES		//This is for solving contiguous memory for you (may have problems with nested stuff)
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		//depth testing, configure matrix to range of 0 to 1 instead of -1 to 1
#define GLM_ENABLE_EXPERIMENTAL			//to use with hash functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>		//rotate, lookAt, perspective
#include <glm/gtx/hash.hpp>

//error reporting
#include <iostream>
#include <stdexcept>

//lambda functions / resource management
#include <functional>

//macros for exit sucess and failure
#include <cstdlib>

//math / extras
#include <assert.h>	//need this for NDEBUG i think
#include <vector>
#include <map>
#include <cstring>
#include <set>
#include <optional>	//a wrapper that has no value until you assign something to it
#include <cstdint>  //needed for UINT32_MAX
#include <algorithm>	//max and min
#include <fstream>		//shader loading
#include <array>
#include <chrono>		//use for time, this can determine math is the same regaurdless of framerate
#include <unordered_map>	//use for vertex duplication on 3d models

#include <iostream>



const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;


//validation layers set up
const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
	//"VK_LAYER_KHRONOS_validation"  //this is the one the tutorial has, i found the other one works, 
									 //all it is is a standard set of layers to pull from, may give troubles down the line
									 //this comes from here:
											//https://vulkan.lunarg.com/doc/view/1.0.13.0/windows/layers.html
};

//list of required device Extensions
const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};



#ifdef NDEBUG	//C++ way of saying not in debug mode
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif



struct QueueFamilyIndices
{
	//for GPU pick and utils
	std::optional<uint32_t> graphicsFamily;

	//for the window surface and glfw abstraction
	std::optional<uint32_t> presentFamily;

	//bool check for device queue family when picking a GPU
	//used in isDeviceSuitable and rateDeviceSuitability
	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


//details to hold the data found for swapchain requirements
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};
