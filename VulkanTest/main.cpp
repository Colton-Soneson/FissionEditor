//from lunarG, gives all funcs enums and structs
//#include <vulkan/vulkan.h>	

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
#include <cstring>
#include <cstdlib>

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
	void initWindow() {
		
		//needed by GLFW
		glfwInit();

		//for non opengl 
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		
		//we resize shit later, so turn that off
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		//create the window
		//	fourth param is for opening on a specific monitor, fifth is only for OpenGL stuff
		mpWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "VulkanAnimal3D", nullptr, nullptr);
	}

	void createInstance() {

		//do validation layers stuff
		if (enableValidationLayers && !checkValidationLayerSupport())
		{
			throw std::runtime_error("Validation Layers requested, but not available");
		}
		 
		//set the app info (pretty self explanitory)
		mAppInfo = {};
		mAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;	//this is what pNext can point to for extensions (default val here leaves as nullptr)
		mAppInfo.pApplicationName = "Triangle";
		mAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		mAppInfo.pEngineName = "No Engine";
		mAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		mAppInfo.apiVersion = VK_API_VERSION_1_0;		//maybe need to set this to _1_1
	
		//not optional driver extensions
		mCreateInfo = {};
		mCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		mCreateInfo.pApplicationInfo = &mAppInfo;

		//include the validation layer names (if enabled)
		if (enableValidationLayers)
		{
			mCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			mCreateInfo.ppEnabledLayerNames = validationLayers.data();	//sets names
		}
		else
		{
			mCreateInfo.enabledLayerCount = 0;	//no layer names
		}

		//agnostic API (extensions to interface with OS required)
		uint32_t glfwExtensionCount = 0;		//maybe make these two member data
		const char** glfwExtensions;

		//GLFW offers built in extension handler
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		mCreateInfo.enabledExtensionCount = glfwExtensionCount;
		mCreateInfo.ppEnabledExtensionNames = glfwExtensions;

		//enables global validator layers
		mCreateInfo.enabledLayerCount = 0;
		
		//finalized
		//VkResult result = vkCreateInstance(&mCreateInfo, nullptr, &mInstance);	//everything can pass to VkResult, it can return the VK_SUCCESS code or errors

		//alternative not storing result
		if (vkCreateInstance(&mCreateInfo, nullptr, &mInstance) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed To create instance");
		}
		else
		{
			std::cout << "instance created";
		}

		//go through extension array
		vkEnumerateInstanceExtensionProperties(nullptr, &mExtensionCount, nullptr);

		//allocate array to hold extension details
		std::vector<VkExtensionProperties> extensions(mExtensionCount);
		mExtensions = new std::vector<VkExtensionProperties>(mExtensionCount); //this is me trying to make it a member 

		vkEnumerateInstanceExtensionProperties(nullptr, &mExtensionCount, extensions.data());

		//go through the VkExtensionProperties (just for proof)
		std::cout << "\n\navailable extensions: \n";
		for (const auto& extension : extensions)
		{
			std::cout << "\t" << extension.extensionName << std::endl;
		}

		//you can also go through glfwGetRequiredInstanceExtensions

	}

	void initVulkan() {
		createInstance();
	}

	void mainLoop() {

		
		while (!glfwWindowShouldClose(mpWindow))
		{
			glfwPollEvents(); //the min rec for this, keep it running till we get polled for an error
		}
	}

	void cleanup() {
		//clean up the instance right before program exit
		vkDestroyInstance(mInstance, nullptr);

		//remove the window
		glfwDestroyWindow(mpWindow);

		//cleanup call for glfw
		glfwTerminate();
	}

	//additional functions
	//checks if all requested layers are available
	bool checkValidationLayerSupport()
	{
		//find total number of layers and pass it into layerCount
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		//go through em
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		//check all layers in validationLayers exist in availableLayers
		for (const char* layerName : validationLayers)
		{
			bool layerFound = false; //basic flag

			for (const auto& layerProperties : availableLayers)
			{

				//name comparison check
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			//we couldnt find a validationLayer inside the available layers we have
			if (layerFound == false)
			{
				return false;
			}
		}

		//all VLs are in ALs
		return true;
	}

	//getters
	//inline const int getWindowWidth() { return mWindowWidth; };
	//inline const int getWindowHeight() { return mWindowHeight; };


	//*********************//
	//     Member Data     //
	//*********************//

	GLFWwindow* mpWindow;

	VkInstance mInstance;
	VkApplicationInfo mAppInfo;
	VkInstanceCreateInfo mCreateInfo;

	uint32_t mExtensionCount = 0;
	std::vector<VkExtensionProperties>* mExtensions;

};



int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}