#pragma once

//ImGUI running on vulkan and glfw

#include "Source/ImGUI/imgui.h"
#include "Source/ImGUI/imgui_impl_glfw.h"
#include "Source/ImGUI/imgui_impl_vulkan.h"


//manual glfw, will automatically load the vulkan.h alongside it
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

//GLM linear alg stuff
#define GLM_FORCE_RADIANS	//makes it so shit like rotate uses radians instead of eulerAngles
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES		//This is for solving contiguous memory for you (may have problems with nested stuff)
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		//depth testing, configure matrix to range of 0 to 1 instead of -1 to 1
#define GLM_ENABLE_EXPERIMENTAL			//to use with hash functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>		//rotate, lookAt, perspective
#include <glm/gtx/hash.hpp>

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
#include <string>
#include <set>
#include <optional>	//a wrapper that has no value until you assign something to it
#include <cstdint>  //needed for UINT32_MAX
#include <algorithm>	//max and min
#include <fstream>		//shader loading
#include <array>
#include <chrono>		//use for time, this can determine math is the same regaurdless of framerate
#include <unordered_map>	//use for vertex duplication on 3d models

#include <iostream>



const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int OPTIONS_WINDOW_WIDTH = 1400;
const int OPTIONS_WINDOW_HEIGHT = 1400;


//validation layers set up
const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
	//"VK_LAYER_NV_nsight"
	//"VK_LAYER_KHRONOS_validation"  //this is the one the tutorial has, i found the other one works, 
									 //all it is is a standard set of layers to pull from, may give troubles down the line
									 //this comes from here:
											//https://vulkan.lunarg.com/doc/view/1.0.13.0/windows/layers.html
};

//list of required device Extensions
const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};



#ifdef _DEBUG	//C++ way of saying not in debug mode
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
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



//make sure to place "Engine *Engine::instance = 0" on the main file
class Engine
{
	static Engine *instance;

public:
	static Engine* getInstance()
	{
		if (!instance)
		{
			instance = new Engine;
			return instance;
		}
	}
};



//VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool)
//{
//	//temp command buffer (can be optimized at some point with VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
//	// for this temp crap)
//	VkCommandBufferAllocateInfo allocInfo = {};
//	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//	allocInfo.commandPool = commandPool;
//	allocInfo.commandBufferCount = 1;
//
//	VkCommandBuffer commandBuffer;
//	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
//
//	VkCommandBufferBeginInfo beginInfo = {};
//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;	//tells the driver we are using CB once and wait with returning 
//																	//	until we are done copying
//
//	vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//	return commandBuffer;
//}
//
//
//void endSingleTimeCommands(VkDevice device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VkCommandPool commandPool)
//{
//	vkEndCommandBuffer(commandBuffer);	//all we need to do is hold the copy, so i ends here
//
////execute buffer to complete transfer
//	VkSubmitInfo submitInfo = {};
//	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//	submitInfo.commandBufferCount = 1;
//	submitInfo.pCommandBuffers = &commandBuffer;
//
//	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//	vkQueueWaitIdle(graphicsQueue);	//we can do the fences thing here instead (come back to if you have time, can do multiple transfers)
//	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);	//wipe the buffer after the copy over
//}
//
//
//uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
//	VkPhysicalDeviceMemoryProperties memProperties;
//	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//
//	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//		if ((typeFilter & (1 << i)) &&		//means that the Bit we have for thihs type is 1 (good to go)
//			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)	//back when we did the bitwise for HOST_VISIBLE and HOT_COHERENT this is what it was for
//		{
//			return i;
//		}
//	}
//	throw std::runtime_error("failed to find suitable memory type");
//}
//
//
//void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
//{
//	VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
//
//	//barrier can also be used to transition image layouts and tansfer queuefamily ownership
//	VkImageMemoryBarrier barrier = {};
//	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//	barrier.oldLayout = oldLayout;
//	barrier.newLayout = newLayout;
//
//	//if used to transfer queue fam ownership, these should be the indices of the queue fam,
//	//	otherwise call the ignore on it (not default feat)
//	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//
//	barrier.image = image;
//	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//	barrier.subresourceRange.baseMipLevel = 0;
//	barrier.subresourceRange.levelCount = mipLevels;
//	barrier.subresourceRange.baseArrayLayer = 0;
//	barrier.subresourceRange.layerCount = 1;		//only one layer defined because image isnt array
//
//	VkPipelineStageFlags sourceStage;
//	VkPipelineStageFlags destinationStage;
//
//	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
//	{
//		barrier.srcAccessMask = 0;
//		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;	//specify earliest possible stage for pre-barrier ops
//		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;	//not a real stage within Graphics and Compute pipelines 
//															//	its a psuedostage for transfers
//	}
//	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
//	{
//		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//	}
//	else
//	{
//		throw std::invalid_argument("unsupported layout transition");
//	}
//
//	//you need to specify which types of operations involving resources must happen
//	//	before tha barrier
//	//barrier.srcAccessMask = 0;
//	//barrier.dstAccessMask = 0;
//
//	vkCmdPipelineBarrier(
//		commandBuffer,
//		sourceStage, destinationStage,
//		0,
//		0, nullptr,
//		0, nullptr,
//		1, &barrier
//	);
//
//	endSingleTimeCommands(device, commandBuffer, graphicsQueue, commandPool);
//}
//
//
//void createImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
//	VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
//{
//	VkImageCreateInfo imageInfo = {};
//	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//	imageInfo.imageType = VK_IMAGE_TYPE_2D;		//what kind of coordinate system the texels (texture pixel) in the image
//												//	are going to be addressed
//	imageInfo.extent.width = width;
//	imageInfo.extent.height = height;
//	imageInfo.extent.depth = 1;					//texels in extent need a depth of atleast 1 not zero
//	imageInfo.mipLevels = mipLevels;					//tex not an array? use 1
//	imageInfo.arrayLayers = 1;					//tex not an array? use 1
//	imageInfo.format = format;					//specified for copy operation of texels as pixels
//	imageInfo.tiling = tiling;					//LINEAR or OPTIMAL
//	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//	imageInfo.usage = usage;					//we use it as a destination to transfer (DST) from copy 
//	imageInfo.samples = numSamples;				//default is set to 1_bit if not adjusted, its multisampling
//	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;	//just by queue family
//	//imageInfo.flags = 0;						//optional: sparse images, where only certain regions are actually backed in memory
//
//	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create image");
//	}
//
//	VkMemoryRequirements memReqs;
//	vkGetImageMemoryRequirements(device, image, &memReqs);
//
//	//works the same way as allocating memory for a buffer
//	VkMemoryAllocateInfo allocInfo = {};
//	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//	allocInfo.allocationSize = memReqs.size;
//	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memReqs.memoryTypeBits, properties);
//
//	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to allocate image memory");
//	}
//
//	vkBindImageMemory(device, image, imageMemory, 0);
//}
//
//
//VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
//{
//	//similar to create image views
//	VkImageViewCreateInfo viewInfo = {};
//	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//	viewInfo.image = image;
//	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//	viewInfo.format = format;
//	viewInfo.subresourceRange.aspectMask = aspectFlags;
//	viewInfo.subresourceRange.baseMipLevel = 0;
//	viewInfo.subresourceRange.levelCount = mipLevels;
//	viewInfo.subresourceRange.baseArrayLayer = 0;
//	viewInfo.subresourceRange.layerCount = 1;
//
//	VkImageView imageView;
//
//	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create texture image view");
//	}
//
//	return imageView;
//}
//
//
//
////general buffer info and memory alloc/ bind creation
//void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
//{
//	//this function can just be called inside buffer creations
//
//	//since this is a generalized helper function, based upon our parameters we set up this info
//	VkBufferCreateInfo bufferInfo = {};
//	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//	bufferInfo.size = size;
//	bufferInfo.usage = usage;
//	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//	//create buffer based on the info
//	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create buffer");
//	}
//
//	VkMemoryRequirements memReqs;
//	vkGetBufferMemoryRequirements(device, buffer, &memReqs);	//get staging requirements (memory shit)
//
//	VkMemoryAllocateInfo allocInfo = {};
//	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//	allocInfo.allocationSize = memReqs.size;
//	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memReqs.memoryTypeBits, properties);
//
//	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to allocate buffer memory");
//	}
//
//	vkBindBufferMemory(device, buffer, bufferMemory, 0);	//now we do the binding after memory allocation
//}
//
//
//void copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
//{
//	VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
//
//	VkBufferCopy copyRegion = {};	//not possible to use VK_WHOLE_SIZE
//	//copyRegion.srcOffset = 0;	
//	//copyRegion.dstOffset = 0;
//	copyRegion.size = size;
//
//	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion); //the transfer
//
//	endSingleTimeCommands(device, commandBuffer, graphicsQueue, commandPool);
//}
//
//
////called before finishing createTextureImage
//void copyBufferToImage(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
//{
//	VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
//
//	//specify which part of the buffer is going to be copied
//	VkBufferImageCopy region = {};
//	region.bufferOffset = 0;		//byte offset at which pixel value starts
//	region.bufferRowLength = 0;		//how laid in memory (padding and whatnot)
//	region.bufferImageHeight = 0;	//how laid in memory
//
//	// subresource, offset, and extent below signify where we want to copy pixels
//
//	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//	region.imageSubresource.mipLevel = 0;
//	region.imageSubresource.baseArrayLayer = 0;
//	region.imageSubresource.layerCount = 1;
//
//	region.imageOffset = { 0, 0, 0 };
//	region.imageExtent = { width, height, 1 };	//last one is depth
//
//	//fourth param here indicates which layout the image is currently using 
//	//	we just assume here that we already transitioned to the layout optimal for copying pixels
//	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
//
//	endSingleTimeCommands(device, commandBuffer, graphicsQueue, commandPool);
//}

