#pragma once

/*
	In order to avoid redefinitions of Vulkan standard paramters for these functions, they needed to be away from the application instances
	if this was above a class in a header file, then a subsequent cpp file to accompany the functions definitions for that class that use the functions below
	would then accidentally redefine said functions below

	don't make a cpp file for this, keep all this data and definitions in this header alone. Its kind of like what you do with templates from what I can see.
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <optional>	//a wrapper that has no value until you assign something to it
#include <vector>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;

	//bool check for device queue family when picking a GPU
	bool isComplete()
	{
		return graphicsFamily.has_value();
	}
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
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

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}


//look up address for extension to DebugUtilsMessengerEXT
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	//the actual address lookup, if it exists itll return the DebugUtilsMessengerEXT otherwise its nullptr
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	//make sure the address is filled with said function and not blank
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}


//destructor for debug messenger extension
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
	else
	{
		throw std::runtime_error("couldnt locate and destroy debug utils messenger EXT");
	}
}