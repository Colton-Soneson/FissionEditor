#include "HelloTriangleApplication.h"
#include "DebugTools.h"

bool HelloTriangleApplication::checkValidationLayerSupport()
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


void HelloTriangleApplication::cleanup()
{
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
	}

	//clean up the instance right before program exit
	vkDestroyInstance(mInstance, nullptr);

	//remove the window
	glfwDestroyWindow(mpWindow);

	//cleanup call for glfw
	glfwTerminate();
}


void HelloTriangleApplication::createInstance() {

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
	/*
	//this got replaced
	if (enableValidationLayers)
	{
		mCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		mCreateInfo.ppEnabledLayerNames = validationLayers.data();	//sets names
	}
	else
	{
		mCreateInfo.enabledLayerCount = 0;	//no layer names
	}
	*/

	//debug messaging
	auto extensionsDebug = getRequiredExtensions();
	mCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensionsDebug.size());	//set extensions count in info
	mCreateInfo.ppEnabledExtensionNames = extensionsDebug.data();	//put actual data into create info

	//final process
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	
	//if debugging is on
	if (enableValidationLayers)
	{
		//enables global validator layers
		mCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		mCreateInfo.ppEnabledLayerNames = validationLayers.data();

		//fill debug
		populateDebugMessengerCreateInfo(debugCreateInfo);
		mCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		//otherwise we have no layers because nother is there
		mCreateInfo.enabledLayerCount = 0;
		mCreateInfo.pNext = nullptr;
	}

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

	//go through extensions props manually inside (OVERWRITTEN)
	/*
	//go through extension array
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	//allocate array to hold extension details
	std::vector<VkExtensionProperties> extensionsProps(extensionCount);
	mpExtensionsProps = new std::vector<VkExtensionProperties>(extensionCount); //this is me trying to make it a member 

	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsProps.data());

	//go through the VkExtensionProperties (just for proof)
	std::cout << "\n\navailable extensions: \n";
	for (const auto& extension : extensionsProps)
	{
		std::cout << "\t" << extension.extensionName << std::endl;
	}

	//you can also go through glfwGetRequiredInstanceExtensions
	*/
}


VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApplication::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	//this all now gets handled in setupDebugMessenger, but this is another simpler way of doing it without
	//the callbacks, its easy and fast
	/*
	//cerr (like c error) is basically access to an error stream and can be loaded with <<
	std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

	//this style allows for comparison statements what to do for what level of message
	//you can find what each had in Vulkan Part 4 of notes or on validation layers chapter
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		std::cout << "\nWATCH OUT\n";
	}

	//setting up debug info
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
										VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
										VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	
	//filter what types to be notified about 
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
									VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
									VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	//you can also pass pUserData via the param
	debugCreateInfo.pfnUserCallback = debugCallback;

	debugCreateInfo.pUserData = nullptr;

	*/

	return VK_FALSE;
}


std::vector<const char*> HelloTriangleApplication::getRequiredExtensions()
{
	//agnostic API (extensions to interface with OS required)
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	//GLFW offers built in extension handler
	//get num of extensions in instance, and set the number of them on the uint32_t
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//get the actual extensions and put them in a vector
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers == true)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}


void HelloTriangleApplication::initVulkan()
{
	createInstance();
	setupDebugMessenger();
}


void HelloTriangleApplication::initWindow() 
{

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


void HelloTriangleApplication::mainLoop() 
{
	while (!glfwWindowShouldClose(mpWindow))
	{
		glfwPollEvents(); //the min rec for this, keep it running till we get polled for an error
	}
}


void HelloTriangleApplication::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo)
{
	//setting up debug info
	debugCreateInfo = {};
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	//filter what types to be notified about 
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	//you can also pass pUserData via the param
	debugCreateInfo.pfnUserCallback = debugCallback;
}


void HelloTriangleApplication::setupDebugMessenger()
{
	if (enableValidationLayers == false) return;

	//create the debuge util info extension
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

	//use the populate function to set standards
	populateDebugMessengerCreateInfo(debugCreateInfo);

	//check to make sure that its running properly
	if (CreateDebugUtilsMessengerEXT(mInstance, &debugCreateInfo, nullptr, &mDebugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}


}
