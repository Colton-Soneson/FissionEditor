#pragma once

#include "NetworkingController.h"
#include <map>

class OptionsWindow
{
public:
	
	OptionsWindow(const int w_size, const int h_size, const int mode, Scene* scene)
	{
		mWindowHeight = h_size;
		mWindowWidth = w_size;
		mMode = mode;
		mShowDemoMenu = false;
		mShowLightMenu = false;
		mShowObjectMenu = false;
		mShowAnimationMenu = false;
		mShowNetworkingMenu = false;
		mScene = scene;
		mpChatHistoryClient = new std::vector<std::string>;
		mpServerHistory = new std::vector<std::string>;
		mpClientCommands = new std::queue<ObjectCommandQueueData>;
		mpIncomingClientsWhoNeedScene = new std::queue<RakNet::SystemAddress>;
		mpNetworkManager = new NetworkManager(mpServerHistory, mpChatHistoryClient, mpClientCommands, mpIncomingClientsWhoNeedScene);
		mObjectHasBeenChanged = false;
		mObjectHasBeenAdded = false;
		mObjectHasBeenDeleted = false;
	}

	~OptionsWindow() {};


	void prerun();

	void run();	//this has to be put inside the while loop of the main program, or make a seperate thread for it later

	bool getObjectAddedStatus() { return mObjectHasBeenAdded; };
	void setObjectAddedStatus(bool tmp) { mObjectHasBeenAdded = tmp; };

	bool getObjectChangedStatus() { return mObjectHasBeenChanged; };
	void setObjectChangedStatus(bool tmp) { mObjectHasBeenChanged = tmp; };

	bool getObjectDeletedStatus() { return mObjectHasBeenDeleted; };
	void setObjectDeletedStatus(bool tmp) { mObjectHasBeenDeleted = tmp; };

private:

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	static void glfw_resize_callback(GLFWwindow*, int w, int h);

	void selectDevice();

	void initDescriptorPool();

	void initWindow();

	void drawFrame();

	void presentFrame();

	// options for networking
	void networkingOptions(bool &showMenu);
	void serverAddAllObjectsToNewClient(RakNet::SystemAddress newClientAddress);

	std::vector<int> getInput(bool active);	//active determines whether or not to have active search for keys


	VkInstance mInstance;
	VkSurfaceKHR mSurface;
	ImGui_ImplVulkanH_Window* mpWindow;
	GLFWwindow* mpGLFWWindow;
	int mWindowHeight;
	int mWindowWidth;
	int mMode;
	int mMinImageCount = 2;
	DeviceSelection* mpDevSel;
	VkQueue mGraphicsQueue;
	VkPhysicalDevice mPhysicalDevice;
	VkQueue mPresentQueue;
	VkDevice mDevice;
	uint32_t mQueueFamGraphicsIndices;
	VkDescriptorPool mDescriptorPool;	//This is essentially the set limits given by demo
	ImGui_ImplVulkanH_Window mMainWindowData;



	//object manipulation
	Scene* mScene;
	bool mShowObjectMenu;
	bool mShowLightMenu;
	bool mShowAnimationMenu;
	bool mShowDemoMenu;
	bool mShowNetworkingMenu;

	bool mObjectHasBeenAdded;
	bool mObjectHasBeenChanged;
	bool mObjectHasBeenDeleted;

	std::vector<int> mInputFromKeyboard;
	

	//networking
	NetworkManager* mpNetworkManager;
	std::vector<std::string>* mpChatHistoryClient;
	std::vector<std::string>* mpServerHistory;
	std::queue<RakNet::SystemAddress>* mpIncomingClientsWhoNeedScene;
	std::queue<ObjectCommandQueueData>* mpClientCommands;
	bool mCurrentlyAClient = false;								//we have this out here, because our normal functionality for networking gets integrated with out editor
};