#pragma once

//manual glfw, will automatically load the vulkan.h alongside it
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

//***************//
//    const      //
//***************//

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


#ifdef NDEBUG	//C++ way of saying not in debug mode
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif



class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();	//preparation
		mainLoop();
		cleanup();
	}

private:
	//we need initWindow because GLFW stock is OpenGL and we need to tell
	//it not to do that
	void initWindow();

	void createInstance();

	void initVulkan();

	void setupDebugMessenger();

	void mainLoop();

	void cleanup();

	//additional functions
	//checks if all requested layers are available
	bool checkValidationLayerSupport();

	//returns list of extensions if validation layers are enabled
	std::vector<const char*> getRequiredExtensions();

	//just a fast way to fill a debugCreateInfo
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);

	//choosing a GPU to do the job
	void pickPhysicalDevice();

	//logical device creation to link it to the physical GPU
	void createLogicalDevice();
	
	//checks if GPU is suitable to perform operations we want
	//returns true if is supported and has a geometry shader
	bool isDeviceSuitable(VkPhysicalDevice device);

	//more methodical way of picking a GPU, gives a score based on the 
	//available features at hand, trumps isDeviceSuitable
	int rateDeviceSuitability(VkPhysicalDevice device);

	//static
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT messageType,
														const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														void* pUserData);


	//*********************//
	//     Member Data     //
	//*********************//

	//GLFW
	GLFWwindow* mpWindow;

	//Instance creation
	VkInstance mInstance;
	VkApplicationInfo mAppInfo;
	VkInstanceCreateInfo mCreateInfo;

	//logical to physical
	VkDevice mDevice;
	VkDeviceQueueCreateInfo mDeviceQueueCreateInfo;
	VkPhysicalDeviceFeatures mDeviceFeatures = {};	//when we first use this place the {} somewhere else
													//this allows us to get the shit like geometry shaders
	VkDeviceCreateInfo mDeviceCreateInfo;
	VkQueue mGraphicsQueue;

	//Debugging
	VkDebugUtilsMessengerEXT mDebugMessenger;

	//storing extension property information (for myself)
	std::vector<VkExtensionProperties>* mpExtensionsProps;

	VkPhysicalDevice mPhysicalDevice;

};
