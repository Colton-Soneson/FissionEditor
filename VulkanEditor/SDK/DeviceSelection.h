#pragma once
#include "Engine.h"

class DeviceSelection
{
public:

	DeviceSelection(GLFWwindow* w, VkSurfaceKHR& s, VkInstance& i)
		: mSurface(s), mInstance(i)
	{
		mpWindow = w;
	}
	
	~DeviceSelection() {};

	void initDevice();

	VkDevice& selectedDevice() { return mDevice; };
	VkPhysicalDevice& selectedPhysicalDevice() { return mPhysicalDevice; };
	VkQueue& graphicsQueue() { return mGraphicsQueue; };
	VkQueue& presentQueue() { return mPresentQueue; };
	QueueFamilyIndices fQueFam() { return findQueueFamilies(mPhysicalDevice, mSurface); };
	SwapChainSupportDetails swapSupDet() { return querySwapChainSupport(mPhysicalDevice, mSurface); };

private:

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

	//be able to check how high we can go on antialiasing/ multisampling
	VkSampleCountFlagBits getMaxUsableSampleCount();

	////find Queue families for varies GPU shit
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	
	std::vector<VkDeviceQueueCreateInfo> mDeviceQueueCreateInfos;	//was a single value
	VkPhysicalDeviceFeatures mDeviceFeatures;	//when we first use this place the {} somewhere else
													//this allows us to get the shit like geometry shaders
	VkDeviceCreateInfo mDeviceCreateInfo;
	VkQueue mGraphicsQueue;

	VkPhysicalDevice mPhysicalDevice;
	VkQueue mPresentQueue;

	VkSampleCountFlagBits mMSAASamples = VK_SAMPLE_COUNT_1_BIT;

	VkDevice mDevice;

	//from outside
	GLFWwindow* mpWindow;
	VkSurfaceKHR& mSurface;
	VkInstance& mInstance;
		
};