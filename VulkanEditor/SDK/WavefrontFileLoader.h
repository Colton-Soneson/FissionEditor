#pragma once

#include <filesystem>
#include <sstream>

#include "ModelData.h"


class WavefrontFileLoader
{
public:

	WavefrontFileLoader(std::string modelFilepath, std::string textureFilepath, VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool)
	{
		mModelFilePath = modelFilepath;
		mTextureFilePath = textureFilepath;
		mDevice = device;
		mPhysicalDevice = physicalDevice;
		mCommandPool = commandPool;
		mGraphicsQueue = graphicsQueue;
	};
	
	~WavefrontFileLoader() {};


	void startLoadingFromFile();

	std::vector<sourced3D> getLoadedObjects();


private:

	//image loading into buffers
	void createTextureImage();

	//access through image views rather than directly
	void createTextureImageView();

	//sampler stuff
	void createTextureSampler();

	//model loading
	void loadModel();

	//vertex buffers
	void createVertexBuffer();

	//index buffer (very similar to createVertexBuffer)
	void createIndexBuffer();

	//mipmap generation, uses a mem barrier and CB
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t textureWidth, int32_t textureHeight, uint32_t mipLevels);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////


	VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);


	void endSingleTimeCommands(VkDevice device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VkCommandPool commandPool);


	uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

	void createImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);


	VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);



	//general buffer info and memory alloc/ bind creation
	void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);


	void copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);


	//called before finishing createTextureImage
	void copyBufferToImage(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	std::vector<sourced3D> mOriginals;

	std::string mModelFilePath;
	std::string mTextureFilePath;

	VkDevice mDevice;
	VkPhysicalDevice mPhysicalDevice;
	VkCommandPool mCommandPool;
	VkQueue mGraphicsQueue;
};