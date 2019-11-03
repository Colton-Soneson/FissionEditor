#include "HelloTriangleApplication.h"
#include "DebugTools.h"

bool HelloTriangleApplication::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	//go through ext props of the device
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);

	//go through ext prop list 
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	//https://en.cppreference.com/w/cpp/container/set
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	//go through available extensions list and as keep erasing out them from required Extensions 
	//until we finally know that every single one of the extensinos we need are fulfilled by the card
	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}


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


VkExtent2D HelloTriangleApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	//gonna be equal to resolution to window we draw to so use GLFW window height and width
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		//push our GLFW window size into extent struct
		VkExtent2D actualExtent = { WINDOW_WIDTH, WINDOW_HEIGHT };

		//get the extents of the surface bounds and sets them in actualExtent 
		//max and min clamp width and height between the supported implementation
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
	
		return actualExtent;
	}
}


VkPresentModeKHR HelloTriangleApplication::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	//Four modes:
	//		VK_PRESENT_MODE_IMMEDIATE_KHR
	//		VK_PRESENT_MODE_FIFO_KHR
	//		VK_PRESENT_MODE_FIFO_RELAXED_KHR
	//		VK_PRESENT_MODE_MAILBOX_KHR

	for (const auto& availablePresentMode : availablePresentModes)
	{
		//This allows for triple buffering, so its preffered
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	//this is the only garaunteed one
	return VK_PRESENT_MODE_FIFO_KHR;
}


VkSurfaceFormatKHR HelloTriangleApplication::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	//color space, using VK_FORMAT_B8G8R8A8_UNORM as a color base instead of SRGB because its easy
	//	that means 8 bits per each B R G (different order RGB) and A channels for a 32 bit color pixel
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	//the [0] is for if we want to have a rating system like for GPU selection
	return availableFormats[0];
}


void HelloTriangleApplication::cleanup()
{
	//pipeline destruction
	vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);

	//renderpass destruction
	vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

	//we need to destroy images via loop through
	for (auto imageView : mSwapChainImageViews)
	{
		vkDestroyImageView(mDevice, imageView, nullptr);
	}

	//destroy swapchain
	vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);

	//device destruction
	vkDestroyDevice(mDevice, nullptr);

	//debug util destruction
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
	}

	//destroy the surface abstraction
	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

	//clean up the instance right before program exit
	vkDestroyInstance(mInstance, nullptr);

	//remove the window
	glfwDestroyWindow(mpWindow);

	//cleanup call for glfw
	glfwTerminate();
}


void HelloTriangleApplication::createGraphicsPipeline()
{
	auto vertShaderCode = readFile("../shaders/vert.spv");
	auto fragShaderCode = readFile("../shaders/frag.spv");

	//create the modules (same way for both vetex and frag)
	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	//vertex shader graphics pipeline object fill
	//	sType here describes which pipeline stage we are on
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

	//allows for combination of multiple fragment shaders into one shader module and 
	// use different entry points for their behaviours, but instead just use the "main"
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	//frag shader
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	//array to contain them
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	//formatting the vertex data
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	
	//fill this shit later
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

	//How we draw vertices from data
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;   //true will break up lines and triangles in the _STRIP topolgy modes using index of 0xFFFF or 0xFFFFFFF

	//viewport (region of framebuffer output renders to)
	VkViewport viewport = {};

	//top left corner
	viewport.x = 0.0f;
	viewport.y = 0.0f;

	//bottom right corner (use what the swapchain defines these as)
	viewport.width = (float)mSwapChainExtent.width;
	viewport.height = (float)mSwapChainExtent.height;

	//depth shit (has to be within 0 an 1, but min may be higher then max
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	//ignore the scissor filter by just drawing the full framebuffer
	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = mSwapChainExtent;

	//combining the viewport and scissor
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;	//possiblility depending on card to have more than one viewport
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	//depth testing, face culling, wireframe rendering
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;				//true, frags past near or far planes are clamped instead of discarding. Requires GPU feature
	rasterizer.rasterizerDiscardEnable = VK_FALSE;		//true, geometry never passes through rasterizer phase (nothing gets to framebuffer)
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;		//(FILL, LINE, or POINT) for entire poly filled, draw wireframe, or draw just points
	rasterizer.lineWidth = 1.0f;						//thickness defined by number of fragments
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;			//opt
	rasterizer.depthBiasClamp = 0.0f;					//opt
	rasterizer.depthBiasSlopeFactor = 0.0f;				//opt


	//Multisampling (anti-aliasing is sharper edges and without running frag shader a lot)
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;			//opt
	multisampling.pSampleMask = nullptr;			//opt
	multisampling.alphaToCoverageEnable = VK_FALSE; //opt
	multisampling.alphaToOneEnable = VK_FALSE;		//opt

	//DEPTH STENCIL TESTING ALSO DONE IN HERE

	//color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;		//opt
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;	//opt
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;				//opt
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;		//opt
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;	//opt
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;				//opt

	//psuedocode for a way to color blend
	/*if (blendEnable) {
		finalColor.rgb = (srcColorBlendFactor * newColor.rgb) < colorBlendOp > (dstColorBlendFactor * oldColor.rgb);
		finalColor.a = (srcAlphaBlendFactor * newColor.a) < alphaBlendOp > (dstAlphaBlendFactor * oldColor.a);
	}
	else {
		finalColor = newColor;
	}

	finalColor = finalColor & colorWriteMask;*/

	//second way is simpler, uses alpha blending
	//finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
	//finalColor.a = newAlpha.a;

	//the above is done like here
	//colorBlendAttachment.blendEnable = VK_TRUE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	//color blending for all of the framebuffers
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;


	//uniform values for pipeline
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;

	//push constants can pass dynamic values to shaders
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout");
	}


	//MAKE SURE THIS IS LAST
	vkDestroyShaderModule(mDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(mDevice, vertShaderModule, nullptr);

}


void HelloTriangleApplication::createImageViews()
{
	//be able to fit all the image views to be creating (same size as available images)
	mSwapChainImageViews.resize(mSwapChainImages.size());

	for (size_t i = 0; i < mSwapChainImages.size(); ++i)
	{
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = mSwapChainImages[i];

		//specification on how data should be interpritted
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;	//you can treat an image as 1D, 2D, 3D, or cube maps
		imageViewCreateInfo.format = mSwapChainImageFormat;

		//default mapping setting for these (you can do 0 to 1)
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		//subresource range describes image purpose and what should be accessed
		//		stereographic 3D requires multiple layers
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;


		//actually create the image view and store it in the vector
		if (vkCreateImageView(mDevice, &imageViewCreateInfo, nullptr, &mSwapChainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views");
		}
	}
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


void HelloTriangleApplication::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);

	//creation of a queue from both families
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;	//the priority here pased by reference to be changed as queue expands
		mDeviceQueueCreateInfos.push_back(queueCreateInfo);
	}

	/*
	//this was when mDeviceQueCreateInfo was one value from graphics fam, not queue fam aswell
	//filling the first of many structs for logical device
	mDeviceQueueCreateInfo = {};
	mDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	mDeviceQueueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	mDeviceQueueCreateInfo.queueCount = 1;	//we wont really need more than one, because CBs can be created on multithreaded then submitted via main thread
	
	
	float queuePriority = 1.0f;		//priority is a value between 0.0 and 1.0
	mDeviceQueueCreateInfo.pQueuePriorities = &queuePriority;
	*/

	mDeviceFeatures = {};
	
	//from this point on its very similar to VkInstanceCreateInfo but with device shit
	mDeviceCreateInfo = {};
	mDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	//these two lines were modded to be for both graphics and present (surfacing) families
	mDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(mDeviceQueueCreateInfos.size());
	mDeviceCreateInfo.pQueueCreateInfos = mDeviceQueueCreateInfos.data();
	
	mDeviceCreateInfo.pEnabledFeatures = &mDeviceFeatures;

	//to enable VK_KHR_swapchain extension
	mDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	mDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();


	//Again, this way is how to do it without specific device extensions (like it was for the instance)
	if (enableValidationLayers == true)
	{
		mDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		mDeviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		mDeviceCreateInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(mPhysicalDevice, &mDeviceCreateInfo, nullptr, &mDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device");
	}

	vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);					//What? mGraphicsQueue is NULL so how can this function be called***************

	vkGetDeviceQueue(mDevice, indices.presentFamily.value(), 0, &mPresentQueue);
}


void HelloTriangleApplication::createRenderPass()
{
	//single color buffer attachment
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = mSwapChainImageFormat;		//same format as from swapchain
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;	//without multisampling its 1 sample
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;	//before rendering
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;	//after rendering
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		//means we dont care about previous layout
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;	//we want it to be ready for presentation after rendering

	//subpass p1: referrencing attachments in framebuffer and optimizing them
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;		//which attachment to reference in atachment descriptiion array
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;	//which layout we would like to have during subpass
																				//this means we get color performance
	//subpass p2
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;	//GPU subpasses instead of CPU
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
			//pInputAttachments, pResolveAttachments, pDepthStencilAttachment, and pPreserveAttachments all also possible

	//renderpass
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;	//would be a list if we had more I imagine
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(mDevice, &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass");
	}
}


VkShaderModule HelloTriangleApplication::createShaderModule(const std::vector<char>& code)
{
	//based upon the SPIR-V code imported from file
	//	we just want to delete it in here because its just easier, so no member data
	VkShaderModuleCreateInfo shaderCreateInfo = {};
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.codeSize = code.size();
	shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	//we are gonna return from this func so dont make member function
	VkShaderModule shaderModule;

	if (vkCreateShaderModule(mDevice, &shaderCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module");
	}

	return shaderModule;
}


void HelloTriangleApplication::createSurface()
{
	if (glfwCreateWindowSurface(mInstance, mpWindow, nullptr, &mSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface");
	}
}


void HelloTriangleApplication::createSwapChain()
{
	//the properties are finally used
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(mPhysicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	//there is a minimum image count we need to fullfill in a swapchain
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;	//we do +1 so we can have atleast one more than the minimum 

	//
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	//structure filling like usual
	mSwapChainCreateInfo = {};
	mSwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	mSwapChainCreateInfo.surface = mSurface;

	//this has to come after surface def (more details in the defining functions)
	mSwapChainCreateInfo.minImageCount = imageCount;
	mSwapChainCreateInfo.imageFormat = surfaceFormat.format;
	mSwapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	mSwapChainCreateInfo.imageExtent = extent;
	mSwapChainCreateInfo.imageArrayLayers = 1;								//ALWAYS ONE unless developing a stereoscopic 3D application
	mSwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;	//specifies swapchain operation type to transfer the rendered image

	//get all the indices for graphics fam and presentation queue
	QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	//draw fon images in the swap chain from graphics queue then submit to presentation queue
	if (indices.graphicsFamily != indices.presentFamily)
	{
		mSwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;	//no explicit ownership transfer
		mSwapChainCreateInfo.queueFamilyIndexCount = 2;
		mSwapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		mSwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;	//image owned by one queue fam and needs explicit transfer to another fam
		mSwapChainCreateInfo.queueFamilyIndexCount = 0;
		mSwapChainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	//defining you want the transform to be, this is just the standard call
	mSwapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	//alpha channel used to blend with other windows in window system, this call ignores that option
	mSwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;


	mSwapChainCreateInfo.presentMode = presentMode;
	mSwapChainCreateInfo.clipped = VK_TRUE;				//performance option, enable unless pixel reading and prediction required

	//given time the swapchain may become unoptimized during runtime
	mSwapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	//the actual creation
	if (vkCreateSwapchainKHR(mDevice, &mSwapChainCreateInfo, nullptr, &mSwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swapchain");
	}

	//fill the vector of images from the swapchain
	vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, nullptr);
	mSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, mSwapChainImages.data());

	//later change these
	mSwapChainImageFormat = surfaceFormat.format;
	mSwapChainExtent = extent;

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


QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	//typical vulkan enumeration
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	//create the queuefams vector by qFC, and then get their data to fill it
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());



	//this will go through the queueFamilies, we need to make sure that 
	//we have support for VK_QUEUE_GRAPHICS_BIT
	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		//surface support
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}

	return indices;
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
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
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


bool HelloTriangleApplication::isDeviceSuitable(VkPhysicalDevice device)
{
	/*
	//this allows us to get name and vk version support (props) as well as cool features (features)
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	//if GPU is discrete and has a geometry shader we good fam send it
	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
	*/
	
	QueueFamilyIndices indices = findQueueFamilies(device);

	//checking for EXT support to do swapchain 
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	//swapchain support check
	bool swapChainAdequate = false;

	if (extensionsSupported)
	{
		//returned from query make sure that the formats are good and present modes have support
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}


	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}


void HelloTriangleApplication::mainLoop() 
{
	while (!glfwWindowShouldClose(mpWindow))
	{
		glfwPollEvents(); //the min rec for this, keep it running till we get polled for an error
	}
}


void HelloTriangleApplication::pickPhysicalDevice()
{
	mPhysicalDevice = VK_NULL_HANDLE;

	//like how we handle listing extensions
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

	//checking if there is support
	if (deviceCount == 0)
	{
		throw std::runtime_error("No GPUs with Vulkan Support Present");
	}

	//if nothing was thrown continue

	//create the list of devices
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

	//go through list of devices, and if one is suitable to do Vulkan Ops then break out
	//this is first come first serve, it wont choose the "best" device if there are multiple
	//GPUs, it'll just do what it can with what it first finds to work
	/*
	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			mPhysicalDevice = device;
			break;
		}
	}

	//if we couldnt get a graphics card that can handle Vulkan
	if (mPhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Couldn't find suitable GPU");
	}
	*/

	//this is a score based system instead of a "first come first serve"
	//type of deal
	
	std::multimap<int, VkPhysicalDevice> candidates;

	for (const auto& device : devices)
	{
		int deviceScore = rateDeviceSuitability(device);

		//make_pair is a C++ 17 term, its like a dictionary in python
		candidates.insert(std::make_pair(deviceScore, device));
	}

	//the rateDeviceSuitability will only return 0 if the device isnt
	//suitable AT ALL
	//top line will give the biggest value of the candidates and as long as its bigger than zero we set the device
	if (candidates.rbegin()->first > 0)
	{
		mPhysicalDevice = candidates.rbegin()->second;
	}
	else
	{
		throw std::runtime_error("Couldn't find suitable GPU");
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


SwapChainSupportDetails HelloTriangleApplication::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	//returns into the struct, core of all these functions are parameters VkPhysicalDevice and VkSurfaceKHR 
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);

	//query list of structs of supported surface formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

	//if there is shit in there
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.data());
	}

	//same way as above for presentation modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.presentModes.data());
	}



	return details;
}


int HelloTriangleApplication::rateDeviceSuitability(VkPhysicalDevice device)
{
	//i strayed from tutorial here, they wanted to keep using the device suitable thing
	//but i want to use this so here we are. 
	QueueFamilyIndices indices = findQueueFamilies(device);

	//numbers here are based on my own system of value, its very hard coded
	int deviceScore = 0;

	//this allows us to get name and vk version support (props) as well as cool features (features)
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	//discrete is better than a hybrid type GPU
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		deviceScore += 1000;
	}

	//max texture size in the 2D space
		//the .limits part has a bunch of things good for scoring
	deviceScore += deviceProperties.limits.maxImageDimension2D;

	//checking for EXT support to do swapchain 
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	//swapchain support check
	bool swapChainAdequate = false;

	if (extensionsSupported)
	{
		//returned from query make sure that the formats are good and present modes have support
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	//if you dont have a geometry shader for this app everything will break
	if (!deviceFeatures.geometryShader || indices.isComplete() == false || extensionsSupported == false || swapChainAdequate == false)		//LOOK: here is where i added the queue check compliance
	{
		return 0;
	}


	return deviceScore;
}


std::vector<char> HelloTriangleApplication::readFile(const std::string& filename)
{
	//ate: start reading at eof
	//binary: read file as binary
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	//by starting at the end of the file we can determine size based on position
	size_t filesize = (size_t)file.tellg();
	std::vector<char> buffer(filesize);

	//go back to beginning, then read all the bytes at once
	file.seekg(0);
	file.read(buffer.data(), filesize);

	//close the file
	file.close();

	return buffer;
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
