#pragma once
//#include "DeviceSelection.h"

#include "OptionsWindow.h"

class DemoApplication {
public:

	friend class OptionsWindow;

	void run() {
		initWindow();
		initScene();
		initVulkan();	//preparation
		initGUIWindow();
		mainLoop();
		cleanup();
	}


	//Scene
	Scene* mScene;

private:
	

	//we need initWindow because GLFW stock is OpenGL and we need to tell
	//it not to do that
	void initWindow();

	void initScene();		//initial Scene, not that big of a deal just make it 3 rotating cubes

	void initGUIWindow();

	void createInstance();

	void selectDevice();

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

	//GLFW dependent surface creation
	void createSurface();

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

	//index buffer (very similar to createVertexBuffer)
	void createIndexBuffer();

	//Descriptor layout for during pipeline creation
	void createDescriptorSetLayout();

	//create uniform buffers for MVP or skeletal animation stuff
	void createUniformBuffers();

	//create uniform buffer 
	void updateUniformBuffer(uint32_t currentImage);

	//we need to allocate from a pool like CBs
	void createDescriptorPool();

	//allocate descriptor sets
	void createDescriptorSets();

	//image loading into buffers
	void createTextureImage();

	//loading buffer into image objects
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
							VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	//record and execute command buffer again
	VkCommandBuffer beginSingleTimeCommands();

	//end cb
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	//layout transitions to finish the vkCmdCopyBufferToImage command
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	
	//called before finishing createTextureImage
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	//access through image views rather than directly
	void createTextureImageView();

	//generalized createImageView function (imageViews/ textureImageViews)
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	//sampler stuff
	void createTextureSampler();

	//basically we set stuff up with image, memory, and image view
	void createDepthResource();

	//find support for format of depth testing
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	//select format with depth component that supporst usage as depth attachment
	VkFormat findDepthFormat();

	//if chosen depth has stencil component return true
	bool hasStencilComponent(VkFormat format);

	//model loading
	void loadModel();

	//mipmap generation, uses a mem barrier and CB
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t textureWidth, int32_t textureHeight, uint32_t mipLevels);

	////be able to check how high we can go on antialiasing/ multisampling
	//VkSampleCountFlagBits getMaxUsableSampleCount();

	//multisampled color buffer
	void createColorResources();

	//for shadowmapping
	void createDepthSampler();

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


	//apply user interaction from the options menu




	//getters
	//Scene& getScene() { return mScene; };

	//*********************//
	//     Member Data     //
	//*********************//

	//consts
	const int MAX_FRAMES_IN_FLIGHT = 2;
	

	//AUTOMATED VERTICES AND INDICES
	//std::vector<Vertex> mVertices;
	//std::vector<uint32_t> mIndices;

	//GLFW
	GLFWwindow* mpWindow;

	//ImGUI Vulkan
	OptionsWindow* mpOpWindow;


	//Instance creation
	VkInstance mInstance;
	VkApplicationInfo mAppInfo;
	VkInstanceCreateInfo mCreateInfo;

	//surfacing
	VkSurfaceKHR mSurface;

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
	VkDescriptorSetLayout mDescriptorSetLayout;
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

	VkQueue mGraphicsQueue;
	VkPhysicalDevice mPhysicalDevice;
	VkQueue mPresentQueue;
	VkDevice mDevice;

	VkDescriptorPool mDescriptorPool;


	//depth testing
	VkImage mDepthImage;
	VkDeviceMemory mDepthImageMemory;
	VkImageView mDepthImageView;
	VkSampler mDepthImageSampler;

	//msaa
	VkSampleCountFlagBits mMSAASamples = VK_SAMPLE_COUNT_2_BIT;
	VkImage mColorImage;		//this is to store desired samples per pixel
	VkDeviceMemory mColorImageMemory;
	VkImageView mColorImageView;

	//Debugging
	VkDebugUtilsMessengerEXT mDebugMessenger;

	//storing extension property information (for myself)
	std::vector<VkExtensionProperties>* mpExtensionsProps;

	//VkPhysicalDevice mPhysicalDevice;
	DeviceSelection* mpDevSel;


};
