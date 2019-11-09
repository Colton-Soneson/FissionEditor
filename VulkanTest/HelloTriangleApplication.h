#pragma once

//manual glfw, will automatically load the vulkan.h alongside it
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

/*
	Git is a stupid fucking piece of fucking shit
	i took extensive fucking notes on this fucking section and they all fucking vanished because
	git couldnt handle a fucking file over 100 mbs so i had to do a revert on the commit tree to update
	lfs and then ALL THIS FUCKING SHIT GOT LOST. fuck git. i learned this.
*/
struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		/*
		float: VK_FORMAT_R32_SFLOAT
		vec2: VK_FORMAT_R32G32_SFLOAT
		vec3: VK_FORMAT_R32G32B32_SFLOAT
		vec4: VK_FORMAT_R32G32B32A32_SFLOAT
		*/

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};


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

	//find Queue families for varies GPU shit
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

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

	//part of swapchain stuff, also fits into device choice
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	//GLFW dependent surface creation
	void createSurface();

	//to fill data in struct for requirements of swapchain
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	//SC PROP 1: surface format detailing search
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	//SC PROP 2: Presentation modes: the only mode garaunteed is VK_PRESENT_MODE_FIFO_KHR
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	//SC PROP 3: resolution of the swap chain images (usually just resolution of the window)
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	//finally create the swapchain given the properties functions
	void createSwapChain();

	//basic image views for every image in swapchain to use them as color targets
	void createImageViews();

	//graphicsPipleine creation (vertex shader -> tesselation -> geometry shader ...)
	void createGraphicsPipeline();

	//a wrapper for SPIR-V code to get into the pipeline
	VkShaderModule createShaderModule(const std::vector<char>& code);

	//framebuffer attachments used while rendering
	void createRenderPass();

	//currently set up for renderpas to expect a single framebuffer with same format as swapchain images
	void createFramebuffers();

	//command pools for mem management of command buffers
	void createCommandPool();

	//records and allocates commands for each swap chain image
	void createCommandBuffers();

	//finally draw the frame
	void drawFrame();

	//sync checking (both semaphores (for GPU-GPU) and fences (for CPU_GPU))
	void createSyncObjects();

	//recreating the swapchain if window resizes to not crash
	void recreateSwapChain();

	//for recreateSC and also cutting into regular cleanup
	void cleanupSwapChain();

	//vertex buffers
	void createVertexBuffer();

	//gets the type of memory used by GPU
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	//general buffer info and memory alloc/ bind creation
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
						VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	//general copy command used in place of vkMapMemory (we can use device local mem types)
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	//----------------------//
	//		static stuff	//
	//----------------------//
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT messageType,
														const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														void* pUserData);

	//shader loading
	static std::vector<char> readFile(const std::string& filename);

	//resizing (has to be static because GLFW cant call member function with "this" pointer
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);



	//*********************//
	//     Member Data     //
	//*********************//

	//consts
	const int MAX_FRAMES_IN_FLIGHT = 2;

	
	const std::vector<Vertex> mVertices = {
	{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	//GLFW
	GLFWwindow* mpWindow;

	//Instance creation
	VkInstance mInstance;
	VkApplicationInfo mAppInfo;
	VkInstanceCreateInfo mCreateInfo;

	//logical to physical
	VkDevice mDevice;
	std::vector<VkDeviceQueueCreateInfo> mDeviceQueueCreateInfos;	//was a single value
	VkPhysicalDeviceFeatures mDeviceFeatures;	//when we first use this place the {} somewhere else
													//this allows us to get the shit like geometry shaders
	VkDeviceCreateInfo mDeviceCreateInfo;
	VkQueue mGraphicsQueue;

	//surfacing
	VkSurfaceKHR mSurface;
	VkQueue mPresentQueue;

	//swapchain
	VkSwapchainCreateInfoKHR mSwapChainCreateInfo;
	VkSwapchainKHR mSwapChain;
	VkFormat mSwapChainImageFormat;
	VkExtent2D mSwapChainExtent;

	//Image
	std::vector<VkImage> mSwapChainImages;
	std::vector<VkImageView> mSwapChainImageViews;

	//final pipeline
	VkRenderPass mRenderPass;
	VkPipelineLayout mPipelineLayout;
	VkPipeline mGraphicsPipeline;

	//framebuffers
	std::vector<VkFramebuffer> mSwapChainFrameBuffers;

	//command buffers
	VkCommandPool mCommandPool;
	std::vector<VkCommandBuffer> mCommandBuffers;

	//Semaphores for sync
	VkSemaphore mImageAvailableSemaphore;
	VkSemaphore mRenderFinishedSemaphore;

	//Frame Delay and Waiting (with semaphores)
	std::vector<VkSemaphore> mImageAvailableSemaphores;
	std::vector<VkSemaphore> mRenderFinishedSemaphores;
	std::vector<VkFence> mInFlightFences;	//to handle CPU-GPU sync
	std::vector<VkFence> mImagesInFlight;	//to prevent rendering images already in flight
	size_t mCurrentFrame = 0;	//to keep track of when to use right semaphore
	bool mFrameBufferResized = false;	//flags when resize of window happens


	//vertex buffers
	VkBuffer mVertexBuffer;
	VkDeviceMemory mVertexBufferMemory;

	//Debugging
	VkDebugUtilsMessengerEXT mDebugMessenger;

	//storing extension property information (for myself)
	std::vector<VkExtensionProperties>* mpExtensionsProps;

	VkPhysicalDevice mPhysicalDevice;

};
