#include "OptionsWindow.h"
#include <sstream>

static bool mSwapChainRebuild = false;
static int mSwapChainResizeWidth = 0;
static int mSwapChainResizeHeight = 0;

VKAPI_ATTR VkBool32 VKAPI_CALL OptionsWindow::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	//this all now gets handled in setupDebugMessenger, but this is another simpler way of doing it without
	//the callbacks, its easy and fast
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


	return VK_FALSE;
}


void OptionsWindow::selectDevice()
{

	//set the app info (pretty self explanitory)
	VkApplicationInfo mAppInfo = {};
	mAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;	//this is what pNext can point to for extensions (default val here leaves as nullptr)
	mAppInfo.pApplicationName = "OptionsWindow";
	mAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	mAppInfo.pEngineName = "No Engine";
	mAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	mAppInfo.apiVersion = VK_API_VERSION_1_0;		//maybe need to set this to _1_1

	//not optional driver extensions
	VkInstanceCreateInfo mCreateInfo = {};
	mCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	mCreateInfo.pApplicationInfo = &mAppInfo;


	//debug messaging
	//auto extensionsDebug = getRequiredExtensions();
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

	auto extensionsDebug = extensions;

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
		mCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		//otherwise we have no layers because nother is there
		mCreateInfo.enabledLayerCount = 0;
		mCreateInfo.pNext = nullptr;
	}
	

	if (vkCreateInstance(&mCreateInfo, nullptr, &mInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed To create instance");
	}
	else
	{
		std::cout << "\n[options] instance created\n";
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	mpGLFWWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Dear ImGui GLFW+Vulkan example", NULL, NULL);


	//surface creation
	VkResult err = glfwCreateWindowSurface(mInstance, mpGLFWWindow, NULL, &mSurface);
	if (err != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface at 179 OP");
	}

	mpDevSel = new DeviceSelection(mSurface, mInstance);

	mpDevSel->initDevice();


	mDevice = mpDevSel->selectedDevice();
	mGraphicsQueue = mpDevSel->graphicsQueue();
	mPhysicalDevice = mpDevSel->selectedPhysicalDevice();
	mPresentQueue = mpDevSel->presentQueue();
	mQueueFamGraphicsIndices = mpDevSel->getQueueFamGraphicsIndices();
}

void OptionsWindow::initDescriptorPool()
{
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	VkResult err = vkCreateDescriptorPool(mDevice, &pool_info, NULL, &mDescriptorPool);
	if (err != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool, line 39 OP");
	}
}

void OptionsWindow::initWindow()
{
	
	mpWindow->Surface = mSurface;

	// Select Surface Format
	const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
	const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	mpWindow->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(mPhysicalDevice, mpWindow->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

	// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
	VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
	VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
	mpWindow->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(mPhysicalDevice, mpWindow->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
	//printf("[vulkan] Selected PresentMode = %d\n", mpWindow->PresentMode);

	// Create SwapChain, RenderPass, Framebuffer, etc.
	IM_ASSERT(mMinImageCount >= 2);
	ImGui_ImplVulkanH_CreateWindow(mInstance, mPhysicalDevice, mDevice, mpWindow, mQueueFamGraphicsIndices, NULL, mWindowWidth, mWindowHeight, mMinImageCount);
	
}

void OptionsWindow::drawFrame()
{
	
	VkResult err;

	VkSemaphore image_acquired_semaphore = mpWindow->FrameSemaphores[mpWindow->SemaphoreIndex].ImageAcquiredSemaphore;
	VkSemaphore render_complete_semaphore = mpWindow->FrameSemaphores[mpWindow->SemaphoreIndex].RenderCompleteSemaphore;
	err = vkAcquireNextImageKHR(mDevice, mpWindow->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &mpWindow->FrameIndex);
	if (err != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to aquire next image, line 47 OW");
	}

	ImGui_ImplVulkanH_Frame* frame = &mpWindow->Frames[mpWindow->FrameIndex];
	{
		err = vkWaitForFences(mDevice, 1, &frame->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
		if (err != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to wait for fences, line 55 OW");
		}

		err = vkResetFences(mDevice, 1, &frame->Fence);
		if (err != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to reset fences, line 61 OW");
		}
	}
	{
		err = vkResetCommandPool(mDevice, frame->CommandPool, 0);
		if (err != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to reset command pool, line 68 OW");
		}

		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(frame->CommandBuffer, &info);
		if (err != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin command buffer, line 77 OW");
		}
	}
	{
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = mpWindow->RenderPass;
		info.framebuffer = frame->Framebuffer;
		info.renderArea.extent.width = mpWindow->Width;
		info.renderArea.extent.height = mpWindow->Height;
		info.clearValueCount = 1;
		info.pClearValues = &mpWindow->ClearValue;
		vkCmdBeginRenderPass(frame->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	// Record Imgui Draw Data and draw funcs into command buffer
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frame->CommandBuffer);

	// Submit command buffer
	vkCmdEndRenderPass(frame->CommandBuffer);
	{
		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &image_acquired_semaphore;
		info.pWaitDstStageMask = &wait_stage;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &frame->CommandBuffer;
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = &render_complete_semaphore;

		err = vkEndCommandBuffer(frame->CommandBuffer);
		if (err != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end CB, line 112 OW");
		}
		err = vkQueueSubmit(mPresentQueue, 1, &info, frame->Fence);
		if (err != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit to queue, line 117 OW");
		}
	}
	
}

void OptionsWindow::presentFrame()
{
	VkSemaphore render_complete_semaphore = mpWindow->FrameSemaphores[mpWindow->SemaphoreIndex].RenderCompleteSemaphore;
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_complete_semaphore;
	info.swapchainCount = 1;
	info.pSwapchains = &mpWindow->Swapchain;
	info.pImageIndices = &mpWindow->FrameIndex;
	VkResult err = vkQueuePresentKHR(mPresentQueue, &info);
	if (err != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Queue present, line 134 OW");
	}
	mpWindow->SemaphoreIndex = (mpWindow->SemaphoreIndex + 1) % mpWindow->ImageCount;
}

void OptionsWindow::glfw_resize_callback(GLFWwindow*, int w, int h)
{
	mSwapChainRebuild = true;
	mSwapChainResizeWidth = w;
	mSwapChainResizeHeight = h;
}

void OptionsWindow::prerun()
{
	
	selectDevice();

	initDescriptorPool();


	// Create Framebuffers
	int w, h;
	glfwGetFramebufferSize(mpGLFWWindow, &w, &h);
	glfwSetFramebufferSizeCallback(mpGLFWWindow, glfw_resize_callback);
	mpWindow = &mMainWindowData;
	initWindow();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForVulkan(mpGLFWWindow, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = mInstance;
	init_info.PhysicalDevice = mPhysicalDevice;
	init_info.Device = mDevice;
	init_info.QueueFamily = mQueueFamGraphicsIndices;
	init_info.Queue = mPresentQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = mDescriptorPool;
	init_info.Allocator = VK_NULL_HANDLE;
	init_info.MinImageCount = mMinImageCount;
	init_info.ImageCount = mpWindow->ImageCount;
	ImGui_ImplVulkan_Init(&init_info, mpWindow->RenderPass);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Upload Fonts
	{
		// Use any command queue
		VkCommandPool command_pool = mpWindow->Frames[mpWindow->FrameIndex].CommandPool;
		VkCommandBuffer command_buffer = mpWindow->Frames[mpWindow->FrameIndex].CommandBuffer;

		VkResult err = vkResetCommandPool(mDevice, command_pool, 0);
		if (err != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to reset command pool");
		}
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(command_buffer, &begin_info);
		if (err != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to reset command pool");
		}

		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &command_buffer;
		err = vkEndCommandBuffer(command_buffer);
		if (err != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end CB");
		}
		err = vkQueueSubmit(mPresentQueue, 1, &end_info, VK_NULL_HANDLE);
		if (err != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit queue");
		}

		err = vkDeviceWaitIdle(mDevice);
		if (err != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to wait, lol");
		}

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	
}

void OptionsWindow::run()
{
	
	// Our state

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	if (!glfwWindowShouldClose(mpGLFWWindow))
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();


		if (mSwapChainRebuild)
		{
			mSwapChainRebuild = false;
			ImGui_ImplVulkan_SetMinImageCount(mMinImageCount);
			ImGui_ImplVulkanH_CreateWindow(mInstance, mPhysicalDevice, mDevice, &mMainWindowData, mQueueFamGraphicsIndices, VK_NULL_HANDLE, mSwapChainResizeWidth, mSwapChainResizeHeight, mMinImageCount);
			mMainWindowData.FrameIndex = 0;
		}
		

		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;
			static bool keyboardInput = false;

			ImGui::Begin("FusionEngine Editor Main Menu");                        
			ImGui::SetWindowFontScale(1.2f);
			ImVec2 winSize(600.0, 450.0);
			ImGui::SetWindowSize(winSize);

			//ImGui::Checkbox("Demo Menu", &mShowDemoMenu);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Object Menu", &mShowObjectMenu);
			ImGui::Checkbox("Light Menu", &mShowLightMenu);
			ImGui::Checkbox("Animation Menu", &mShowAnimationMenu);
			ImGui::Checkbox("Networking Menu", &mShowNetworkingMenu);


			//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			//ImGui::SameLine();
			//ImGui::Text("counter = %d", counter);


			ImGui::Checkbox("TRACK KEYBOARD INPUT", &keyboardInput);
			mInputFromKeyboard = getInput(keyboardInput);

			std::string tempCodes = "";
			std::stringstream inputOutStatement;

			if (!mInputFromKeyboard.empty())
			{
				for (int i = 0; i < mInputFromKeyboard.size(); i++)
				{
					inputOutStatement << mInputFromKeyboard.at(i) << " ";
				}
			}

			std::string fKC = "Current Input KeyCodes: " + inputOutStatement.str();
			ImGui::Text(fKC.c_str());

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		
		static bool readyToCreate = false;
		static bool readyToEdit = false;
		static bool readyToDelete = false;
		static int objectIndex = 0;
		static float posX = 0;
		static float posY = 0;
		static float posZ = 0;
		static float scaleX = 1;
		static float scaleY = 1;
		static float scaleZ = 1;
		static float rotX = 0;
		static float rotY = 0;
		static float rotZ = 0;
		static float ambMod = 0.015f;
		static bool activatelighting = false;
	

		static int objectSelectionAnim = 0;
		static int objectSelectionEdit = 0;
		static int objectSelectionDelete = 0;
		
		//animation stuff
		static const char* channels[]{ "x axis", "y axis", "z axis" };
		static int channelSelection = 0;
		static int keyframeCount = 0;
		static int clipCount = -1;
		static int clipControllerCount = 0;
		static int skeletonCount = -1;
		static int sphereObjectIndex = 0;

		static float justDataPosX = 0.0;
		static float justDataPosY = 0.0;
		static float justDataPosZ = 0.0;
		static float justDataRotX = 0.0;
		static float justDataRotY = 0.0;
		static float justDataRotZ = 0.0;
		static float justDataScaleX = 1.0;
		static float justDataScaleY = 1.0;
		static float justDataScaleZ = 1.0;
		static float justDataDuration = 1.0;		//CANT BE ZERO
		static float justDataFixedDuration = 1.0;	//CANT BE ZERO
		static bool fixedDurationMenu = false;
		static int justDataFirstClipKeyframe = 0;
		static int justDataLastClipKeyframe = 0;

		static bool wholeObjectKeyframingMenu = false;
		static bool skeletalAnimationMenu = false;
		static bool lerpUpdateMode = false;
		static bool keyframeMenu = false;
		static bool clipMenu = false;
		static bool creatingSkeleton = false;
		static std::vector<std::string> clipControllerNames;
		static std::vector<std::pair<bool, int>> clipControllerMenus;
		static std::vector<std::pair<int, std::pair<int, bool>>> clipControllerOptions;		//int, int, bool = clip controller number, the option selected, the options state
		

		// 3. Show another simple window.
		if (mShowLightMenu)
		{
			ImGui::Begin("Light Menu", &mShowLightMenu);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::SetWindowFontScale(1.5);
			ImGui::Text("This menu currently is able to add or delete light objects to the scene");

			if (ImGui::Button("Add Light (WIP, multiple light support not available)"))
			{

			}

			ImGui::Text("");
			ImGui::Text("Current Lights in scene: ");
			for (auto& light : mScene->getLights())
			{
				ImGui::BulletText(light.msName.c_str());
			}


			if (ImGui::Button("Close"))
				mShowLightMenu = false;
			ImGui::End();
		}

		if (mShowObjectMenu)
		{
			ImGui::Begin("Object Menu", &mShowObjectMenu);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::SetWindowFontScale(1.2);
			ImGui::Text("This menu currently is able to add or delete scene objects");

			ImGui::Checkbox("ADD OBJECT MODE", &readyToCreate);

			if (readyToCreate)
			{
				ImGui::Text("CHOSEN OBJECT:");
				ImGui::Text(mScene->getUOSNameByIndex(objectIndex).c_str());
				ImGui::SliderInt("OBJECT INDEX: ", &objectIndex, 0, mScene->getUOSTotalStorageNumber() - 1);

				ImGui::Text("SETTINGS:");
				ImGui::SliderFloat("ScaleX", &scaleX, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
				ImGui::SliderFloat("ScaleY", &scaleY, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
				ImGui::SliderFloat("ScaleZ", &scaleZ, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f

				ImGui::SliderFloat("PositionX", &posX, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
				ImGui::SliderFloat("PositionY", &posY, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
				ImGui::SliderFloat("PositionZ", &posZ, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f

				ImGui::SliderFloat("RotationX", &rotX, 0.0f, 360.0f);            // Edit 1 float using a slider from 0.0f to 360.0f
				ImGui::SliderFloat("RotationY", &rotY, 0.0f, 360.0f);            // Edit 1 float using a slider from 0.0f to 360.0f
				ImGui::SliderFloat("RotationZ", &rotZ, 0.0f, 360.0f);            // Edit 1 float using a slider from 0.0f to 360.0f

				ImGui::Checkbox("Activate Lighting", &activatelighting);
				ImGui::SliderFloat("ambientLighting", &ambMod, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f


				if (ImGui::Button("ADD THE OBJECT"))
				{
					mObjectHasBeenAdded = true;

					//mScene->instantiateObject(objectIndex);
					mScene->instantiateObject(objectIndex, glm::vec3(posX, posY, posZ), glm::vec3(scaleX, scaleY, scaleZ), glm::vec3(rotX, rotY, rotZ), ambMod, activatelighting );

					//networking add object
					if (mCurrentlyAClient)
					{
						mpNetworkManager->clientObjectAddSend(objectIndex, glm::vec3(posX, posY, posZ), glm::vec3(scaleX, scaleY, scaleZ), glm::vec3(rotX, rotY, rotZ), ambMod, activatelighting);
					}

					posX = 0;
					posY = 0;
					posZ = 0;
					scaleX = 1;
					scaleY = 1;
					scaleZ = 1;
					rotX = 0;
					rotY = 0;
					rotZ = 0;
					ambMod = 0.015f;
					readyToCreate = false;
				}
			}

			ImGui::Checkbox("EDIT OBJECT MODE", &readyToEdit);

			if (readyToEdit)
			{
				ImGui::Text("CHOOSE OBJECT:");
				if (mScene->getObjects().size() != 0)
				{
					if (ImGui::Button("Next"))
					{

						objectSelectionEdit++;

						if (objectSelectionEdit >= mScene->getObjects().size())
						{
							objectSelectionEdit = 0;
						}

						glm::mat4 temp = mScene->getObjects().at(objectSelectionEdit).msUBO.model;
						MatrixMath mMath;
						glm::vec3 tempPos = mMath.extractTranslation(temp);
						glm::vec3 tempScale = mMath.extractScale(temp);
						glm::vec3 tempRot = mMath.extractEulerRotation(temp);

						posX = tempPos.x;
						posY = tempPos.y;
						posZ = tempPos.z;
						scaleX = tempScale.x;
						scaleY = tempScale.y;
						scaleZ = tempScale.z;
						rotX = tempRot.x;
						rotY = tempRot.y;
						rotZ = tempRot.z;
						activatelighting = mScene->getObjects().at(objectSelectionEdit).msUBO.activeLight;
						ambMod = mScene->getObjects().at(objectSelectionEdit).msUBO.ambientMod;
					}



					ImGui::Text("Chosen Object:");
					ImGui::Text(mScene->getObjects().at(objectSelectionEdit).msName.c_str());

					bool noChange = true;	//no change has been made to the values
					float nc1, nc2, nc3, nc4, nc5, nc6, nc7, nc8, nc9, nc10;
					bool ncb;

					nc1 = scaleX;
					nc2 = scaleY;
					nc3 = scaleZ;
					ImGui::SliderFloat("ScaleX", &scaleX, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
					ImGui::SliderFloat("ScaleY", &scaleY, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
					ImGui::SliderFloat("ScaleZ", &scaleZ, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f

					nc4 = posX;
					nc5 = posY;
					nc6 = posZ;
					ImGui::SliderFloat("PositionX", &posX, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
					ImGui::SliderFloat("PositionY", &posY, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
					ImGui::SliderFloat("PositionZ", &posZ, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f

					nc7 = rotX;
					nc8 = rotY;
					nc9 = rotZ;
					ImGui::SliderFloat("RotationX", &rotX, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 360.0f
					ImGui::SliderFloat("RotationY", &rotY, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 360.0f
					ImGui::SliderFloat("RotationZ", &rotZ, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 360.0f

					ncb = activatelighting;
					nc10 = ambMod;
					ImGui::Checkbox("Activate Lighting", &activatelighting);
					ImGui::SliderFloat("ambientLighting", &ambMod, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

					if (nc1 != scaleX || nc2 != scaleY || nc3 != scaleZ || nc4 != posX || nc5 != posY || nc6 != posZ
						|| nc7 != rotX || nc8 != rotY || nc9 != rotZ || nc10 != ambMod || ncb != activatelighting)
					{
						noChange = false;
					}

					if (noChange == false)
					{
						mScene->adjustObject(objectSelectionEdit, glm::vec3(posX, posY, posZ), glm::vec3(scaleX, scaleY, scaleZ), glm::vec3(rotX, rotY, rotZ), ambMod, activatelighting);

						if (mCurrentlyAClient)
						{
							mpNetworkManager->clientObjectEditSend(objectSelectionEdit, glm::vec3(posX, posY, posZ), glm::vec3(scaleX, scaleY, scaleZ), glm::vec3(rotX, rotY, rotZ), ambMod, activatelighting);
						}
					}
				}
			}

			ImGui::Checkbox("DELETE OBJECT MODE", &readyToDelete);

			if (readyToDelete)
			{
				ImGui::Text("CHOOSE OBJECT:");
				if (mScene->getObjects().size() != 0)
				{
					if (ImGui::Button("Next"))
					{

						objectSelectionDelete++;

						if (objectSelectionDelete >= mScene->getObjects().size())
						{
							objectSelectionDelete = 0;
						}

						glm::mat4 temp = mScene->getObjects().at(objectSelectionDelete).msUBO.model;
						MatrixMath mMath;
						glm::vec3 tempPos = mMath.extractTranslation(temp);
						glm::vec3 tempScale = mMath.extractScale(temp);
						glm::vec3 tempRot = mMath.extractEulerRotation(temp);

						posX = tempPos.x;
						posY = tempPos.y;
						posZ = tempPos.z;
						scaleX = tempScale.x;
						scaleY = tempScale.y;
						scaleZ = tempScale.z;
						rotX = tempRot.x;
						rotY = tempRot.y;
						rotZ = tempRot.z;
						activatelighting = mScene->getObjects().at(objectSelectionDelete).msUBO.activeLight;
						ambMod = mScene->getObjects().at(objectSelectionDelete).msUBO.ambientMod;
					}



					ImGui::Text("Chosen Object:");
					ImGui::Text(mScene->getObjects().at(objectSelectionDelete).msName.c_str());
					
					
					if (ImGui::Button("REMOVE THE OBJECT"))
					{
						mScene->removeObject(objectSelectionDelete);
						mObjectHasBeenDeleted = true;

						if (mCurrentlyAClient)
						{
							mpNetworkManager->clientObjectDeleteSend(objectSelectionDelete);
						}
					}
				}
			}

			ImGui::Text("");
			ImGui::Text("Current objects in scene: ");

			for (auto& object : mScene->getObjects())
			{
				ImGui::BulletText(object.msName.c_str());
			}
			

			if (ImGui::Button("Close"))
				mShowObjectMenu = false;
			ImGui::End();
		}

		if (mShowAnimationMenu)
		{
			ImGui::Begin("Animation Menu", &mShowAnimationMenu);   
			ImGui::SetWindowFontScale(1.5);

			ImGui::Checkbox("Skeletal Animation", &skeletalAnimationMenu);

			if (skeletalAnimationMenu)
			{

				if (ImGui::Button("Create Basic Humanoid Skeleton"))
				{
					mScene->getSkeletonManager()->createHumanoidBasic();
					skeletonCount++;
					
					sphereObjectIndex = mScene->getIndexFromObjectName("sphere");

					creatingSkeleton = true;
					
				}

				if (ImGui::Button("Create Example HTR Skeleton"))
				{
					mScene->getSkeletonManager()->createHTRSkeleton();
				}

				if (creatingSkeleton)
				{
					mObjectHasBeenAdded = true;
					static int i = 0;
					if (i < mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.at(skeletonCount).mpHierarchy->getNumberOfNodes() - 1)
					{
						//glm::vec3 tempPos = mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.at(skeletonCount).mpHierarchicalPosePool->getHierarchicalPoses().at(0).mHierarchicalJoints.at(i)->mPos;
						//glm::vec3 tempRot = mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.at(skeletonCount).mpHierarchicalPosePool->getHierarchicalPoses().at(0).mHierarchicalJoints.at(i)->mRot;
						//glm::vec3 tempScale = 0.5f * (mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.at(skeletonCount).mpHierarchicalPosePool->getHierarchicalPoses().at(0).mHierarchicalJoints.at(i)->mScale);
						
						glm::vec3 tempPos = mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.at(skeletonCount).mpHierarchicalPosePool->getHierarchicalPoseGroups().mBasePose.mHPG.at(i).mHierarchicalJoint->mPos;
						glm::vec3 tempRot = mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.at(skeletonCount).mpHierarchicalPosePool->getHierarchicalPoseGroups().mBasePose.mHPG.at(i).mHierarchicalJoint->mRot;
						glm::vec3 tempScale = 0.5f * (mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.at(skeletonCount).mpHierarchicalPosePool->getHierarchicalPoseGroups().mBasePose.mHPG.at(i).mHierarchicalJoint->mScale);


						mScene->instantiateObject(sphereObjectIndex, tempPos, tempScale, tempRot, 0.15, false);

						i++;
					}
					else
					{
						i = 0;
						creatingSkeleton = false;
					}
					
				}

				if (!mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.empty())
				{
					//go through list of basic humanoids
					for (int i = 0; i < mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.size(); ++i)
					{
						ImGui::TextColored(ImVec4(0.0, 0.5, 0.0, 1.0), "Sammy The Skeleton");
						ImGui::Text(mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.at(i).getNodeNameList().c_str());

						if (ImGui::Button("Add Base Hierarchical State"))
						{
							mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.at(i).createHierarchicalState();
						}

						if (ImGui::Button("Add Manual Flip Hierarchical State"))
						{
							mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.at(i).createHierarchicalState();
						}

						if (ImGui::Button("Add Base Hierarchical State"))
						{
							mScene->getSkeletonManager()->getSkeletonContainer().mHumanoidBasics.at(i).createHierarchicalState();
						}
						
					}
				}

				//update all skeletons
				//mScene->getSkeletonManager()->update();
				
			}

			ImGui::Checkbox("Entire Object Keyframing", &wholeObjectKeyframingMenu);
			if (wholeObjectKeyframingMenu)
			{
				ImGui::Text("Keyframe & Clip Controller");

				//CREATE AND ADD KEYFRAME TO POOL
				if (ImGui::Button("Create Keyframe"))
				{
					keyframeMenu = true;

					//first set
					glm::mat4 temp = mScene->getObjects().at(objectSelectionAnim).msUBO.model;
					MatrixMath mMath;
					glm::vec3 tempPos = mMath.extractTranslation(temp);
					glm::vec3 tempScale = mMath.extractScale(temp);
					glm::vec3 tempRot = mMath.extractEulerRotation(temp);

					justDataPosX = tempPos.x;
					justDataPosY = tempPos.y;
					justDataPosZ = tempPos.z;
					justDataScaleX = tempScale.x;
					justDataScaleY = tempScale.y;
					justDataScaleZ = tempScale.z;
					justDataRotX = tempRot.x;
					justDataRotY = tempRot.y;
					justDataRotZ = tempRot.z;
					activatelighting = mScene->getObjects().at(objectSelectionAnim).msUBO.activeLight;
					ambMod = mScene->getObjects().at(objectSelectionAnim).msUBO.ambientMod;
				}

				if (keyframeMenu)
				{
					int check = objectIndex;

					ImGui::Text("CHOSEN OBJECT:");
					ImGui::Text(mScene->getObjects().at(objectSelectionAnim).msName.c_str());
					ImGui::SliderInt("OBJECT INDEX: ", &objectIndex, 0, mScene->getObjects().size() - 1);

					if (objectIndex != check)	//update only when we switch to a new object
					{
						glm::mat4 temp = mScene->getObjects().at(objectSelectionAnim).msUBO.model;
						MatrixMath mMath;
						glm::vec3 tempPos = mMath.extractTranslation(temp);
						glm::vec3 tempScale = mMath.extractScale(temp);
						glm::vec3 tempRot = mMath.extractEulerRotation(temp);

						justDataPosX = tempPos.x;
						justDataPosY = tempPos.y;
						justDataPosZ = tempPos.z;
						justDataScaleX = tempScale.x;
						justDataScaleY = tempScale.y;
						justDataScaleZ = tempScale.z;
						justDataRotX = tempRot.x;
						justDataRotY = tempRot.y;
						justDataRotZ = tempRot.z;
						activatelighting = mScene->getObjects().at(objectSelectionAnim).msUBO.activeLight;
						ambMod = mScene->getObjects().at(objectSelectionAnim).msUBO.ambientMod;
					}



					ImGui::InputFloat("pos x axis keyframe:", &justDataPosX);
					ImGui::InputFloat("pos y axis keyframe:", &justDataPosY);
					ImGui::InputFloat("pos z axis keyframe:", &justDataPosZ);
					ImGui::InputFloat("rot x axis keyframe:", &justDataRotX);
					ImGui::InputFloat("rot y axis keyframe:", &justDataRotY);
					ImGui::InputFloat("rot z axis keyframe:", &justDataRotZ);
					ImGui::InputFloat("scale x axis keyframe:", &justDataScaleX);
					ImGui::InputFloat("scale y axis keyframe:", &justDataScaleY);
					ImGui::InputFloat("scale z axis keyframe:", &justDataScaleZ);
					ImGui::InputFloat("duration of keyframe:", &justDataDuration);


					if (justDataDuration <= 0.0)
					{
						ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "Duration Can Not Zero or Negative");
						justDataDuration = 1.0;
					}
					else
					{
						if (ImGui::Button("Add Keyframe to Pool"))
						{

							mScene->addKeyframeToObjectKeyframePool(keyframeCount, justDataDuration,
								glm::vec3(justDataPosX, justDataPosY, justDataPosZ),
								glm::vec3(justDataRotX, justDataRotY, justDataRotZ),
								glm::vec3(justDataScaleX, justDataScaleY, justDataScaleZ));

							++keyframeCount;	//MAKE SURE THIS IS AFTER ADDKEYFRAME

							justDataDuration = 1.0;
							justDataPosX = 0;
							justDataPosY = 0;
							justDataPosZ = 0;
							justDataRotX = 0;
							justDataRotY = 0;
							justDataRotZ = 0;
							justDataScaleX = 1.0;
							justDataScaleY = 1.0;
							justDataScaleZ = 1.0;
							keyframeMenu = false;
						}
					}
				}

				ImGui::Text("_______________________\n");

				//CREATE AND ADD CLIP
				if (keyframeCount < 0)
				{
					ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "No Keyframes to Make Clip From");
				}
				else
				{
					if (ImGui::Button("Create Clip"))
					{
						clipMenu = true;
					}

					if (clipMenu)
					{
						ImGui::SliderInt("First Clip Keyframe:", &justDataFirstClipKeyframe, 0, justDataLastClipKeyframe);
						ImGui::SliderInt("Last Clip Keyframe:", &justDataLastClipKeyframe, justDataFirstClipKeyframe, (mScene->getObjectKeyframePool()->size() - 1));

						ImGui::Checkbox("Fixed Duration", &fixedDurationMenu);

						if (fixedDurationMenu)
						{
							ImGui::InputFloat("Fixed Duration [opt]:", &justDataFixedDuration);
						}

						if (justDataFixedDuration <= 0.0 && fixedDurationMenu)
						{
							ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "Fixed Duration Can Not Zero or Negative");
							justDataDuration = 1.0;
						}
						else
						{
							if (ImGui::Button("Add Clip to Pool"))
							{
								if (fixedDurationMenu == false)
								{
									mScene->addClipToObjectClipPool(justDataFirstClipKeyframe, justDataLastClipKeyframe);
								}
								else
								{
									mScene->addClipToObjectClipPool(justDataFirstClipKeyframe, justDataLastClipKeyframe, justDataFixedDuration);
								}

								++clipCount;

								justDataFirstClipKeyframe = 0;
								justDataLastClipKeyframe = 0;
								justDataFixedDuration = 0.0;
								clipMenu = false;
							}
						}
					}
				}


				ImGui::Text("_______________________\n");

				//ADD CONTROLLER
				if (ImGui::Button("Add Clip Controller"))
				{

					std::string defaultName = "default #";
					defaultName += std::to_string(clipControllerCount);

					//add the controller
					mScene->addObjectClipController(defaultName);

					//clipControllerMenus.push_back(false);
					clipControllerMenus.push_back(std::pair<bool, int>(false, -1));

					//options menu (.at(clip controller number). the option selected. the options state)
								// play/ pause
									//play / pause options
								// set to (start playing at) first / last keyframe
								// change control clip
								// flip playback direction
									//set clip looping or transitional looping
								// slow mo (multiply time step by a factor of less than one)
					clipControllerOptions.push_back(std::pair<int, std::pair<int, bool>>(clipControllerCount, std::pair<int, bool>(0, false)));
					clipControllerOptions.push_back(std::pair<int, std::pair<int, bool>>(clipControllerCount, std::pair<int, bool>(1, false)));
					clipControllerOptions.push_back(std::pair<int, std::pair<int, bool>>(clipControllerCount, std::pair<int, bool>(2, false)));
					clipControllerOptions.push_back(std::pair<int, std::pair<int, bool>>(clipControllerCount, std::pair<int, bool>(3, false)));
					clipControllerOptions.push_back(std::pair<int, std::pair<int, bool>>(clipControllerCount, std::pair<int, bool>(4, false)));
					clipControllerOptions.push_back(std::pair<int, std::pair<int, bool>>(clipControllerCount, std::pair<int, bool>(5, false)));
					clipControllerOptions.push_back(std::pair<int, std::pair<int, bool>>(clipControllerCount, std::pair<int, bool>(6, false)));



					++clipControllerCount;
				}

				ImGui::Text("____________________________");

				if (clipControllerCount > 0)
				{
					for (int i = 0; i < clipControllerCount; ++i)
					{
						if (ImGui::Button(mScene->getObjectClipControllers().at(i)->getName().c_str()))
						{
							clipControllerMenus.at(i).first = !clipControllerMenus.at(i).first;
						}

						if (clipCount >= 0)	//do we have clips to add
						{
							if (clipControllerMenus.at(i).first)
							{
								if (mScene->getObjectClipControllers().at(i)->getClipIndexInPool() == -1)	//if we dont have a clip to control yet
								{
									ImGui::TextColored(ImVec4(0.2, 1.0, 1.0, 1.0), "Choose a Clip To Control");

									//go through all available clips
									for (int j = 0; j <= clipCount; ++j)
									{
										std::string clipName = "Clip #";
										clipName += std::to_string(j);

										if (ImGui::Button(clipName.c_str()))
										{
											mScene->getObjectClipControllers().at(i)->setClipToUseByIndex(j);
											mScene->getObjectClipControllers().at(i)->setStartToFirstKeyframe();
											++clipControllerMenus.at(i).second;								//This is for multiclip control????
											break;
										}
									}
								}
								else
								{
									std::string CtoC = "Using Clip #";
									CtoC += std::to_string(mScene->getObjectClipControllers().at(i)->getClipIndexInPool());
									ImGui::TextColored(ImVec4(0.7, 0.7, 0.0, 1.0), CtoC.c_str());

									//CLIP CONTROLS
									for (int j = 0; j < clipControllerOptions.size(); ++j)
									{
										if (clipControllerOptions.at(j).first == i)	//if our clip controller number is right
										{
											// play/ pause
											if (clipControllerOptions.at(j).second.first == 0)
											{
												ImGui::Indent(8.0f);
												ImGui::Checkbox("\tPlay / Pause Menu", &clipControllerOptions.at(j).second.second);
												if (clipControllerOptions.at(j).second.second == true)
												{
													ImGui::Indent(32.0f);
													ImGui::Checkbox("\tPause the Clip", &clipControllerOptions.at(j + 1).second.second);	//j+1 is to get those play pause options
													if (clipControllerOptions.at(j + 1).second.second == true)
													{
														//set the timestep to 0 (or just set playback direction to pause mode)
														mScene->getObjectClipControllers().at(i)->getPlaybackDirection() = 1;

													}
													ImGui::Indent(-32.0f);
												}
												ImGui::Indent(-8.0f);

											}

											// set to (start playing at) first / last keyframe
											if (clipControllerOptions.at(j).second.first == 2)
											{
												ImGui::Indent(8.0f);
												ImGui::Checkbox("\tstart at first / last keyframe", &clipControllerOptions.at(j).second.second);
												if (clipControllerOptions.at(j).second.second == true)
												{
													ImGui::Indent(32.0f);

													if (ImGui::Button("Start Playing at First Frame"))
													{
														mScene->getObjectClipControllers().at(i)->setStartToFirstKeyframe();
													}

													if (ImGui::Button("Start Playing at Last Frame"))
													{
														mScene->getObjectClipControllers().at(i)->setStartToLastKeyframe();
													}

													ImGui::Indent(-32.0f);
												}
												ImGui::Indent(-8.0f);
											}

											// change control clip
											if (clipControllerOptions.at(j).second.first == 3)
											{
												ImGui::Indent(8.0f);
												ImGui::Checkbox("\tChange Clip", &clipControllerOptions.at(j).second.second);
												if (clipControllerOptions.at(j).second.second == true)
												{
													ImGui::Indent(32.0f);

													ImGui::TextColored(ImVec4(0.2, 1.0, 1.0, 1.0), "Choose a Clip To Control");

													//go through all available clips
													for (int m = 0; m <= clipCount; ++m)
													{
														std::string clipName = "Clip #";
														clipName += std::to_string(m);

														if (ImGui::Button(clipName.c_str()))
														{
															mScene->getObjectClipControllers().at(i)->setClipToUseByIndex(m);
															++clipControllerMenus.at(i).second;								//This is for multiclip control????
															break;
														}
													}

													//get the keyframes in clip
													std::string kfc = "Clip #";
													int clipNum = mScene->getObjectClipControllers().at(i)->getClipIndexInPool();
													kfc += std::to_string(clipNum);
													kfc += " Keyframes: ";
													ImGui::TextColored(ImVec4(0.2, 1.0, 0.0, 1.0), kfc.c_str());

													for (int m = mScene->getObjectClipPool()->getClips().at(clipNum).mFirstKeyframeIndex; m <= mScene->getObjectClipPool()->getClips().at(clipNum).mLastKeyframeIndex; ++m)
													{
														ImGui::TextColored(ImVec4(0.3, 1.0, 0.0, 1.0), mScene->getObjectKeyframePool()->getKeyframe(m).mData.outputPositionString().c_str());
														ImGui::TextColored(ImVec4(0.3, 1.0, 0.0, 1.0), mScene->getObjectKeyframePool()->getKeyframe(m).mData.outputRotationString().c_str());
														ImGui::TextColored(ImVec4(0.3, 1.0, 0.0, 1.0), mScene->getObjectKeyframePool()->getKeyframe(m).mData.outputScaleString().c_str());
													}

													ImGui::Indent(-32.0f);
												}
												ImGui::Indent(-8.0f);
											}

											// flip playback direction
											if (clipControllerOptions.at(j).second.first == 4)
											{
												ImGui::Indent(8.0f);
												ImGui::Checkbox("\tFlip playback direction", &clipControllerOptions.at(j).second.second);
												if (clipControllerOptions.at(j).second.second == true)
												{
													ImGui::Indent(32.0f);

													if (clipControllerOptions.at(0).second.second == true)
													{
														ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "WARNING: This will override the play / pause option");
													}

													ImGui::Text("-1 / 0 / 1  :  backwards / pause / fowards");
													ImGui::SliderInt("Direction: ", &mScene->getObjectClipControllers().at(i)->getPlaybackDirection(), -1, 1);

													ImGui::Checkbox("\tTransitional Clip Looping", &clipControllerOptions.at(j + 1).second.second);
													if (clipControllerOptions.at(j + 1).second.second == true)
													{
														mScene->getObjectClipControllers().at(i)->setTransitionalMode(true);
														std::string temp = "RETRIEVED (multi clip loop): ";
														temp += std::to_string(mScene->getObjectClipControllers().at(i)->getPlaybackDirection());
														ImGui::BulletText(temp.c_str());
													}
													else
													{
														mScene->getObjectClipControllers().at(i)->setTransitionalMode(false);
														std::string temp = "RETRIEVED (single clip loop): ";
														temp += std::to_string(mScene->getObjectClipControllers().at(i)->getPlaybackDirection());
														ImGui::BulletText(temp.c_str());
													}


													ImGui::Indent(-32.0f);
												}
												ImGui::Indent(-8.0f);
											}

											// slow mo (multiply time step by a factor of less than one)
											if (clipControllerOptions.at(j).second.first == 6)
											{
												ImGui::Indent(8.0f);
												ImGui::Checkbox("\tslow mo", &clipControllerOptions.at(j).second.second);
												if (clipControllerOptions.at(j).second.second == true)
												{
													ImGui::Indent(32.0f);
													ImGui::InputFloat("SlowMo Multiplier ( >= 0): ", &mScene->getObjectClipControllers().at(i)->getSlowMoMultiplier());

													if (mScene->getObjectClipControllers().at(i)->getSlowMoMultiplier() < 0)
													{
														ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "WARNING: SloMo has to be above 0");
														mScene->getObjectClipControllers().at(i)->getSlowMoMultiplier() = 1;
													}

													ImGui::Indent(-32.0f);
												}
												ImGui::Indent(-8.0f);
											}

										}
									}




									ImGui::TextColored(ImVec4(0.2, 1.0, 0.0, 1.0), "OUTPUTS");

									std::string time = "Engine Time: ";
									time += std::to_string(mScene->getEngineTimeStep());
									ImGui::TextColored(ImVec4(0.2, 1.0, 0.0, 1.0), time.c_str());

									ImGui::TextColored(ImVec4(0.2, 1.0, 0.0, 1.0), "Current Keyframe (Data |  Index): ");

									ImGui::TextColored(ImVec4(0.3, 1.0, 0.0, 1.0), mScene->getObjectKeyframePool()->getKeyframe(mScene->getObjectClipControllers().at(i)->getCurrentKeyframeIndex()).mData.outputPositionString().c_str());
									ImGui::TextColored(ImVec4(0.3, 1.0, 0.0, 1.0), mScene->getObjectKeyframePool()->getKeyframe(mScene->getObjectClipControllers().at(i)->getCurrentKeyframeIndex()).mData.outputRotationString().c_str());
									ImGui::TextColored(ImVec4(0.3, 1.0, 0.0, 1.0), mScene->getObjectKeyframePool()->getKeyframe(mScene->getObjectClipControllers().at(i)->getCurrentKeyframeIndex()).mData.outputScaleString().c_str());
									std::string currentKeyframe = " ";
									currentKeyframe += " | ";
									currentKeyframe += std::to_string(mScene->getObjectKeyframePool()->getKeyframe(mScene->getObjectClipControllers().at(i)->getCurrentKeyframeIndex()).mIndex);
									ImGui::TextColored(ImVec4(0.2, 1.0, 0.0, 1.0), currentKeyframe.c_str());


								}
							}

							////MULTICLIP CONTROLLING
							//if (clipControllerMenus.at(i).second != -1)
							//{
							//	for (int j = 0; j <= clipControllerMenus.at(i).second; ++j)	//rolls through all the clips that this CC controls
							//	{
							//		ImGui::Text("Clips To Control");
							//	}
							//}
						}
						else
						{
							ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "No Clips in Clip Pool to Add to Controller");
						}


					}
				}


				ImGui::Text("_________________________");

				ImGui::Checkbox("Lerp Update Mode [UNSTABLE] ", &lerpUpdateMode);

				for (int i = 0; i < clipControllerCount; ++i)
				{
					if (mScene->getObjectClipControllers().at(i)->getClipIndexInPool() != -1)	//if we have clips to go through on this controller
					{

						glm::vec3 pos = mScene->getObjectKeyframePool()->getKeyframe(mScene->getObjectClipControllers().at(i)->getCurrentKeyframeIndex()).mData.mPos;
						glm::vec3 rot = mScene->getObjectKeyframePool()->getKeyframe(mScene->getObjectClipControllers().at(i)->getCurrentKeyframeIndex()).mData.mRot;
						glm::vec3 scale = mScene->getObjectKeyframePool()->getKeyframe(mScene->getObjectClipControllers().at(i)->getCurrentKeyframeIndex()).mData.mScale;

						if (lerpUpdateMode)
						{
							mScene->getObjectClipControllers().at(i)->lerpUpdate(1 / mScene->getEngineTimeStep(), pos, rot, scale);
						}
						else
						{
							mScene->getObjectClipControllers().at(i)->update(1 / mScene->getEngineTimeStep());	// 1/60 for 60fps 
						}

						mScene->adjustObject(i, pos, scale, rot, ambMod, activatelighting);
					}
				}

			}

			if (ImGui::Button("Close"))
				mShowAnimationMenu = false;
			ImGui::End();
		}

		networkingOptions(mShowNetworkingMenu);

		// Rendering
		ImGui::Render();
		memcpy(&mpWindow->ClearValue.color.float32[0], &clear_color, 4 * sizeof(float));
		drawFrame();

		presentFrame();
	}
	
}

void OptionsWindow::networkingOptions(bool &showMenu)
{
	static bool ownPersonalServer = false;
	static bool openServerList = false;
	static bool openServerOptions = false;
	static bool openChatWindow = false;
	static bool openServerWindow = false;
	static char clientJoinName[512] = "Colton";
	static char clientJoinServerAddress[512] = "192.168.1.72";
	static char serverPasswordSet[512] = "default_password";
	static char clientPasswordGuess[512] = "default_guess";

	static bool localRemoveFlag = false;
	static bool localAddServerContentFlag = false;
	static int localRemovalCount = -1;
	static int localAddCount = 0;


	if (showMenu)
	{
		ImGui::Begin("Networking Menu", &showMenu);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::SetWindowFontScale(1.0);
		ImGui::Text("This menu is for controlling server and client abilities of this program");
		ImGui::Text(" ");


		//--------------------SERVER----------------------
		ImGui::Text("__________________________SERVER_OPTIONS________________________________");
		ImGui::Checkbox("Create a server", &openServerOptions);
		if (openServerOptions)
		{
			ImGui::InputText("Set Server Password", serverPasswordSet, IM_ARRAYSIZE(serverPasswordSet));

			if (ImGui::Button("Create Server"))
			{
				ownPersonalServer = true;
				mpNetworkManager->initServer(60000, 10, serverPasswordSet);
			}

			if (ImGui::Button("Shutdown Server"))
			{
				ownPersonalServer = false;
				mpNetworkManager->closeServer();
			}

			ImGui::Checkbox("Open Server Status Window", &openServerWindow);

		}
		if (ownPersonalServer)
		{
			ImGui::Text("SERVER IS ONLINE AND UPDATING");
			mpNetworkManager->updateServer();
		}
		ImGui::Text(" ");

		//---------------------CLIENT----------------------
		ImGui::Text("__________________________CLIENT_OPTIONS________________________________");
		ImGui::Checkbox("Connect to a server as client", &openServerList);
		if (openServerList)
		{
			ImGui::InputText("Username", clientJoinName, IM_ARRAYSIZE(clientJoinName));
			ImGui::InputText("Server Address", clientJoinServerAddress, IM_ARRAYSIZE(clientJoinServerAddress));

			ImGui::InputText("Admin Password", clientPasswordGuess, IM_ARRAYSIZE(clientPasswordGuess));

			if (ImGui::Button("Join Server"))
			{
				mCurrentlyAClient = true;

				if (ownPersonalServer == false)
				{
					//when you join a server not your own, wipe all current local scene objects
					if (mScene->getObjects().size() != 0)
					{
						localRemoveFlag = true;
						localRemovalCount = mScene->getObjects().size() - 1;		// i think minus one
					}
				}

				mpNetworkManager->initClient(clientJoinName, 60000, clientJoinServerAddress);
			}

			if (ImGui::Button("Request Admin Access"))
			{
				mpNetworkManager->sendClientAdminRequest(clientPasswordGuess);
			}

			if (ImGui::Button("Ping Yourself (admin only)"))
			{
				char msg[512] = "-pi ";
				strcat(msg, clientJoinName);
				mpNetworkManager->sendClientMessage(msg);
			}
			ImGui::Text(" ");


			ImGui::Checkbox("Open Chat Window", &openChatWindow);


			if (ImGui::Button("Leave Server"))
			{
				mCurrentlyAClient = false;
				mpNetworkManager->closeClient();
			}
		}
		if (mCurrentlyAClient)
		{
			if (localRemoveFlag)
			{
				mScene->removeObject(localRemovalCount);
				localRemovalCount--;
				mObjectHasBeenDeleted = true;

				if (localRemovalCount == -1)
				{
					localRemoveFlag = false;
				}
			}
			else
			{
				ImGui::Text("CLIENT IS ONLINE AND CONNECTING");
				mpNetworkManager->updateClient();

				//UPDATE THE OBJECTS  (MAYBE TRY MULTITHREADING)
				if (!mpClientCommands->empty())
				{
					ObjectCommandQueueData cmd = mpClientCommands->front();

					if (cmd.commandType == (unsigned char)OCQ_OBJECT_ADD)
					{
						mObjectHasBeenAdded = true;
						mScene->instantiateObject(cmd.objectIndex, cmd.pos, cmd.scale, cmd.rot, cmd.ambMod, cmd.activatelighting);
					}
					else if (cmd.commandType == (unsigned char)OCQ_OBJECT_EDIT)
					{
						mScene->adjustObject(cmd.objectIndex, cmd.pos, cmd.scale, cmd.rot, cmd.ambMod, cmd.activatelighting);
					}
					else if (cmd.commandType == (unsigned char)OCQ_OBJECT_REMOVE)
					{

					}
					else if (cmd.commandType == (unsigned char)OCQ_OBJECT_ANIMATE)
					{

					}

					//end with getting first element out
					mpClientCommands->pop();
				}
			}
		}

		ImGui::Text(" ");
		


		if (ImGui::Button("Close"))
			showMenu = false;
		ImGui::End();
	}
	
	if (showMenu && openChatWindow)
	{
		static char chatMsg[512];

		ImGui::Begin("Chat Window", &openChatWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::SetWindowFontScale(1.25);
		
		ImGui::Text("");

		for (std::vector<std::string>::iterator it = begin(*mpChatHistoryClient); it != end(*mpChatHistoryClient); ++it)
		{
			ImGui::BulletText(it->c_str());
		}

		ImGui::InputText("", chatMsg, IM_ARRAYSIZE(chatMsg));

		if (ImGui::Button("send"))
		{
			mpNetworkManager->sendClientMessage(chatMsg);
		}

		if (ImGui::Button("Close"))
			openChatWindow = false;
		ImGui::End();
	}

	if (showMenu && openServerWindow)
	{

		ImGui::Begin("Server Status Window", &openServerWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::SetWindowFontScale(1.25);
		ImGui::Text("");

		for (std::vector<std::string>::iterator it = begin(*mpServerHistory); it != end(*mpServerHistory); ++it)
		{
			ImGui::BulletText(it->c_str());
		}
		

		if (ImGui::Button("Close"))
			openChatWindow = false;
		ImGui::End();
	}
}

void OptionsWindow::addAllObjectsToNewClient()
{

}

std::vector<int> OptionsWindow::getInput(bool active)
{
	std::vector<int> temp;

	if (active)
	{
		if (ImGui::IsKeyDown('W'))
		{
			temp.push_back((int)OWI_W);
		}
		if (ImGui::IsKeyDown('S'))
		{
			temp.push_back((int)OWI_S);
		}
		if (ImGui::IsKeyDown('A'))
		{
			temp.push_back((int)OWI_A);
		}
		if (ImGui::IsKeyDown('D'))
		{
			temp.push_back((int)OWI_D);
		}
		if (ImGui::IsKeyDown('I'))
		{
			temp.push_back((int)OWI_I);
		}
		if (ImGui::IsKeyDown('K'))
		{
			temp.push_back((int)OWI_K);
		}
		if (ImGui::IsKeyDown('J'))
		{
			temp.push_back((int)OWI_J);
		}
		if (ImGui::IsKeyDown('L'))
		{
			temp.push_back((int)OWI_L);
		}
	}
	
	return temp;
}