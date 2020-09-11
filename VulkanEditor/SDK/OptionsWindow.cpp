#include "OptionsWindow.h"
#include "MatrixMath.h"

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
	//init_info.CheckVkResultFn = check_vk_result;
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
		

			ImGui::Begin("FusionEngine Editor Main Menu");                        
			ImGui::SetWindowFontScale(1.2f);
			ImVec2 winSize(600.0, 450.0);
			ImGui::SetWindowSize(winSize);

			ImGui::Checkbox("Demo Menu", &mShowDemoMenu);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Object Menu", &mShowObjectMenu);
			ImGui::Checkbox("Light Menu", &mShowLightMenu);
			ImGui::Checkbox("Animation Menu", &mShowAnimationMenu);

			//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			//ImGui::SameLine();
			//ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		static bool readyToCreate = false;
		static bool readyToEdit = false;
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
	

		static int objectSelection = 0;
		
		//animation stuff
		static const char* channels[]{ "x axis", "y axis", "z axis" };
		static int channelSelection = 0;
		static int keyframeCount = -1;
		static int clipCount = -1;
		static int clipControllerCount = 0;
		
		static float justDataX = 0.0;
		static float justDataY = 0.0;
		static float justDataZ = 0.0;
		static float justDataDuration = 1.0;		//CANT BE ZERO
		static float justDataFixedDuration = 1.0;	//CANT BE ZERO
		static bool fixedDurationMenu = false;
		static int justDataFirstClipKeyframe = 0;
		static int justDataLastClipKeyframe = 0;

		static bool keyframeMenu = false;
		static bool clipMenu = false;
		static std::vector<std::string> clipControllerNames;
		static std::vector<std::pair<bool, int>> clipControllerMenus;

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
				
				if (ImGui::Button("Next"))
				{

					objectSelection++;

					if (objectSelection >= mScene->getObjects().size())
					{
						objectSelection = 0;
					}

					glm::mat4 temp = mScene->getObjects().at(objectSelection).msUBO.model;
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
					activatelighting = mScene->getObjects().at(objectSelection).msUBO.activeLight;
					ambMod = mScene->getObjects().at(objectSelection).msUBO.ambientMod;
				}

				

				ImGui::Text("Chosen Object:");
				ImGui::Text(mScene->getObjects().at(objectSelection).msName.c_str());

				
				ImGui::SliderFloat("ScaleX", &scaleX, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
				ImGui::SliderFloat("ScaleY", &scaleY, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
				ImGui::SliderFloat("ScaleZ", &scaleZ, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
			 
				ImGui::SliderFloat("PositionX", &posX, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
				ImGui::SliderFloat("PositionY", &posY, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
				ImGui::SliderFloat("PositionZ", &posZ, -50.0f, 50.0f);            // Edit 1 float using a slider from -50.0f to 50.0f
				
				ImGui::SliderFloat("RotationX", &rotX, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 360.0f
				ImGui::SliderFloat("RotationY", &rotY, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 360.0f
				ImGui::SliderFloat("RotationZ", &rotZ, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 360.0f
				
				ImGui::Checkbox("Activate Lighting", &activatelighting);
				ImGui::SliderFloat("ambientLighting", &ambMod, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f


				mScene->adjustObject(objectSelection, glm::vec3(posX, posY, posZ), glm::vec3(scaleX, scaleY, scaleZ), glm::vec3(rotX, rotY, rotZ), ambMod, activatelighting);

				//if (ImGui::Button("EDIT THE OBJECT"))
				//{
				//	//mObjectHasBeenChanged = true;

				//}
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

		if (mShowDemoMenu)
		{
			ImGui::ShowDemoWindow(&mShowDemoMenu);
		}

		if (mShowAnimationMenu)
		{
			ImGui::Begin("Animation Menu", &mShowAnimationMenu);   
			ImGui::SetWindowFontScale(1.5);
			ImGui::Text("Keyframe & Clip Controller");

			//CREATE AND ADD KEYFRAME TO POOL
			if (ImGui::Button("Create Keyframe"))
			{
				keyframeMenu = true;
			}

			if (keyframeMenu)
			{
				ImGui::InputFloat("x axis keyframe:", &justDataX);
				ImGui::InputFloat("duration of keyframe:", &justDataDuration);

				//only set up for one float value axis right now
				//ImGui::InputFloat("y axis keyframe:", &justDataY);
				//ImGui::InputFloat("z axis keyframe:", &justDataZ);
				
				if (justDataDuration <= 0.0)
				{
					ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "Duration Can Not Zero or Negative");
					justDataDuration = 1.0;
				}
				else
				{
					if (ImGui::Button("Add Keyframe to Pool"))
					{

						mScene->addKeyframeToKeyframePool(keyframeCount, justDataDuration, justDataX);
						++keyframeCount;	//MAKE SURE THIS IS AFTER ADDKEYFRAME

						justDataDuration = 1.0;
						justDataX = 0;
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
					ImGui::SliderInt("Last Clip Keyframe:", &justDataLastClipKeyframe, justDataFirstClipKeyframe, (mScene->getKeyframePool()->size() - 1));

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
								mScene->addClipToClipPool(justDataFirstClipKeyframe, justDataLastClipKeyframe);
							}
							else
							{
								mScene->addClipToClipPool(justDataFirstClipKeyframe, justDataLastClipKeyframe, justDataFixedDuration);
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
				mScene->addClipController(defaultName);

				//clipControllerMenus.push_back(false);
				clipControllerMenus.push_back(std::pair<bool, int>(false,-1));
				++clipControllerCount;
			}

			ImGui::Text("____________________________");

			if (clipControllerCount > 0)
			{
				for (int i = 0; i < clipControllerCount; ++i)
				{
					if (ImGui::Button(mScene->getClipControllers().at(i)->getName().c_str()))
					{
						clipControllerMenus.at(i).first = !clipControllerMenus.at(i).first;
					}

					if (clipControllerMenus.at(i).first)
					{
						//ADD clip to clip controller
						if (ImGui::Button("add clip to clip controller"))
						{
							
						}
					}

					if (clipControllerMenus.at(i).second != -1)
					{
						for (int j = 0; j <= clipControllerMenus.at(i).second; ++j)
						{
							ImGui::Text("Clips To Control");
						}
					}

					ImGui::Text("_________________________");
				}
			}

			
			/*
			

			ImGui::InputFloat("x axis keyframe:", &justDataX);
			ImGui::InputFloat("y axis keyframe:", &justDataY);
			ImGui::InputFloat("z axis keyframe:", &justDataZ);

			ImGui::Combo("Channel Selection", &channelSelection, channels, IM_ARRAYSIZE(channels), 2);

			if (channelSelection == 0)
			{

			}
			else if (channelSelection == 1)
			{

			}
			else
			{

			}*/
			

			if (ImGui::Button("Close"))
				mShowAnimationMenu = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		memcpy(&mpWindow->ClearValue.color.float32[0], &clear_color, 4 * sizeof(float));
		drawFrame();

		presentFrame();
	}
	
}