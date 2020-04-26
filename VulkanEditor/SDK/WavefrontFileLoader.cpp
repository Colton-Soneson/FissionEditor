#include "WavefrontFileLoader.h"

//stb for image/ texture (reinclude this when doing shit)
//		STB_IMAGE_IMPLEMENTATION MUST BE DEFINED IN HERE, its preprocessor directive states it cant
//		be defined within a .h file
#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
#include <stb/stb_image.h>

//not sure if tinyobjloader is the same way that stb is but do this all the same
#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>
#include <tol/tiny_obj_loader.h>




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



VkCommandBuffer WavefrontFileLoader::beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool)
{
	//temp command buffer (can be optimized at some point with VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
	// for this temp crap)
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;	//tells the driver we are using CB once and wait with returning 
																	//	until we are done copying

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}


void WavefrontFileLoader::endSingleTimeCommands(VkDevice device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VkCommandPool commandPool)
{
	vkEndCommandBuffer(commandBuffer);	//all we need to do is hold the copy, so i ends here

//execute buffer to complete transfer
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);	//we can do the fences thing here instead (come back to if you have time, can do multiple transfers)
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);	//wipe the buffer after the copy over
}


uint32_t WavefrontFileLoader::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) &&		//means that the Bit we have for thihs type is 1 (good to go)
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)	//back when we did the bitwise for HOST_VISIBLE and HOT_COHERENT this is what it was for
		{
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type");
}


void WavefrontFileLoader::transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

	//barrier can also be used to transition image layouts and tansfer queuefamily ownership
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;

	//if used to transfer queue fam ownership, these should be the indices of the queue fam,
	//	otherwise call the ignore on it (not default feat)
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;		//only one layer defined because image isnt array

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;	//specify earliest possible stage for pre-barrier ops
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;	//not a real stage within Graphics and Compute pipelines 
															//	its a psuedostage for transfers
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition");
	}

	//you need to specify which types of operations involving resources must happen
	//	before tha barrier
	//barrier.srcAccessMask = 0;
	//barrier.dstAccessMask = 0;

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	endSingleTimeCommands(device, commandBuffer, graphicsQueue, commandPool);
}


void WavefrontFileLoader::createImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
	VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;		//what kind of coordinate system the texels (texture pixel) in the image
												//	are going to be addressed
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;					//texels in extent need a depth of atleast 1 not zero
	imageInfo.mipLevels = mipLevels;					//tex not an array? use 1
	imageInfo.arrayLayers = 1;					//tex not an array? use 1
	imageInfo.format = format;					//specified for copy operation of texels as pixels
	imageInfo.tiling = tiling;					//LINEAR or OPTIMAL
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;					//we use it as a destination to transfer (DST) from copy 
	imageInfo.samples = numSamples;				//default is set to 1_bit if not adjusted, its multisampling
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;	//just by queue family
	//imageInfo.flags = 0;						//optional: sparse images, where only certain regions are actually backed in memory

	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image");
	}

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(device, image, &memReqs);

	//works the same way as allocating memory for a buffer
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memReqs.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory");
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}


VkImageView WavefrontFileLoader::createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
	//similar to create image views
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;

	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture image view");
	}

	return imageView;
}



//general buffer info and memory alloc/ bind creation
void WavefrontFileLoader::createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	//this function can just be called inside buffer creations

	//since this is a generalized helper function, based upon our parameters we set up this info
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	//create buffer based on the info
	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer");
	}

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(device, buffer, &memReqs);	//get staging requirements (memory shit)

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memReqs.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);	//now we do the binding after memory allocation
}


void WavefrontFileLoader::copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

	VkBufferCopy copyRegion = {};	//not possible to use VK_WHOLE_SIZE
	//copyRegion.srcOffset = 0;	
	//copyRegion.dstOffset = 0;
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion); //the transfer

	endSingleTimeCommands(device, commandBuffer, graphicsQueue, commandPool);
}


//called before finishing createTextureImage
void WavefrontFileLoader::copyBufferToImage(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

	//specify which part of the buffer is going to be copied
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;		//byte offset at which pixel value starts
	region.bufferRowLength = 0;		//how laid in memory (padding and whatnot)
	region.bufferImageHeight = 0;	//how laid in memory

	// subresource, offset, and extent below signify where we want to copy pixels

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };	//last one is depth

	//fourth param here indicates which layout the image is currently using 
	//	we just assume here that we already transitioned to the layout optimal for copying pixels
	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands(device, commandBuffer, graphicsQueue, commandPool);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



std::vector<sourced3D> WavefrontFileLoader::getLoadedObjects()
{
	//check if we dont have any objects in the array, then just output a warning
	
	if (mOriginals.empty())
	{
		std::cout << "\nWARNING: (WavefrontFileLoader) no obj files present in the directory: " << mFilePath << "\n";
	}
	return mOriginals;
	
}

void WavefrontFileLoader::startLoadingFromFile()
{
	//parse through the windows files for names	http://www.cplusplus.com/forum/windows/189681/
	//	create sourced3D objects for each name, set modelpath/ texturepath/ UBO generics along with it 
	//		set the names of the objects to be their filenames

	namespace stdfs = std::filesystem;

	std::vector<std::string> filenames;
	const stdfs::directory_iterator end{};

	for (stdfs::directory_iterator iter{ (stdfs::path)mFilePath }; iter != end; ++iter)
	{
		if (stdfs::is_regular_file(*iter)) // comment out if all names (names of directories tc.) are required
			filenames.push_back(iter->path().string());
	}

	for (const auto& object : filenames)
	{
		sourced3D temp;

		temp.msUBO.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		temp.msUBO.activeLight = 0;
		temp.msUBO.ambientMod = 0.05f;

		std::cout << "filename input: " << object << "/n";


	}


	//do the rest
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	loadModel();
	createVertexBuffer();
	createIndexBuffer();
}

void WavefrontFileLoader::createTextureImage()
{
	for (auto& object : mOriginals)
	{
		int textureWidth, textureHeight, textureChannels;

		//take file path and number of channels to load as args
		stbi_uc* pixels = stbi_load(object.msTexturePath.c_str(),
			&textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);

		//we use 4 because there are 4 bytes per pixel
		VkDeviceSize imageSize = textureWidth * textureHeight * 4;

		//mip level division (explained in notes) but obvious
		object.msMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;
		//object.setMipLevel(static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1);

		if (!pixels) { throw std::runtime_error("failed to load texture image"); };

		//create buffer in host visible memory to use vkMapMem to copy on the pixels
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		createBuffer(mDevice, mPhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);

		//copy those pixel values to the buffer
		void* data;
		vkMapMemory(mDevice, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(mDevice, stagingBufferMemory);

		//free up the pixel array
		stbi_image_free(pixels);

		//mipmapping VkCmdBlit transfer op needs SRC and DST here
		createImage(mDevice, mPhysicalDevice, textureWidth, textureHeight, object.msMipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			object.msTextureImage, object.msTextureImageMemory);

		//we need to transition the texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		//		then execute the buffer to image copy operation
		transitionImageLayout(mDevice, mCommandPool, mGraphicsQueue, object.msTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, object.msMipLevels);
		copyBufferToImage(mDevice, mGraphicsQueue, mCommandPool, stagingBuffer, object.msTextureImage, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));

		//VK_IMAGE_LAYOUT_UNDEFINED image will be considered the "old layout" here
		/*
		transitionImageLayout(mTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
									VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mMipLevels);
		*/
		//clear up memory
		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);

		generateMipmaps(object.msTextureImage, VK_FORMAT_R8G8B8A8_UNORM, textureWidth, textureHeight, object.msMipLevels);
	}
}


void WavefrontFileLoader::createTextureImageView()
{
	for (auto& object : mOriginals)
	{
		object.msTextureImageView = createImageView(mDevice, object.msTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, object.msMipLevels);
	}
}


void WavefrontFileLoader::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	//specifies how to interpolate texels that are magnified or minified
	//	great for what we set up for mipmaps
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	//texture space coords
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	//anistrophic filtering
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;				//lower value is better performace

	//which color is returned when sampling beyond the image WITH CLAMP TO BORDER ADDRESSING MODE
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;		//true means you use coords of 0 to textureWidth/Height
														//false means its 0 to 1

	samplerInfo.compareEnable = VK_FALSE;			//true means texels will first be compared to value 
													//	then the result will be used in filter operation
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	//MIP MAPS ADJUST HERE
	for (auto& object : mOriginals)
	{
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod = 0.0f;				//change values here with static_cast<float>(mipLevels / k); where k is a real number
		samplerInfo.maxLod = static_cast<float>(object.msMipLevels);
		samplerInfo.mipLodBias = 0.0f;

		if (vkCreateSampler(mDevice, &samplerInfo, nullptr, &object.msTextureSampler) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture sampler");
		}
	}
}


void WavefrontFileLoader::loadModel()
{
	for (auto& object : mOriginals)
	{
		tinyobj::attrib_t attrib;	//this holds vertices, normals, and texcoord vectors
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, object.msModelPath.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices = {};	//need to use this for index buffer
																	//	removal of duplicate data

		//combine all faces in file into a single model (triangulation)
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				//using out predefined vertex struct
				Vertex vertex = {};


				vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],	//its stored as array of floats so we 
				attrib.vertices[3 * index.vertex_index + 1],	// need to multiply the index by 3 to compensate
				attrib.vertices[3 * index.vertex_index + 2]		//	for glm::vec3. Offsets 0, 1, and 2 = x, y, and z
				};

				vertex.textureCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1] //had to flip for vert component
				};

				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};

				//std::cout << " " << attrib.normals[3 * index.normal_index + 0] << " " << attrib.normals[3 * index.normal_index + 1] << " " << attrib.normals[3 * index.normal_index + 2] << std::endl;

				vertex.color = { 1.0f, 1.0f, 1.0f };

				//make sure we dont have duplicate vertices data
				//	check if we have already seen same pos (== is overloaded in Vertex)
				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(object.mModel.msVertices.size());
					object.mModel.msVertices.push_back(vertex);
				}

				//only push back non repeated vertices
				object.mModel.msIndices.push_back(uniqueVertices[vertex]);
			}
		}
	}
}


void WavefrontFileLoader::createVertexBuffer() {

	for (auto& object : mOriginals)
	{
		VkDeviceSize bufferSize = sizeof(object.mModel.msVertices[0]) * object.mModel.msVertices.size();

		//actual staging buffer that only host visible buffer as temp buffer (then we later use device local as actual)
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(mDevice, mPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);

		/*
		//without create buffer for staging

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(mVertices[0]) * mVertices.size();	//easy mem byte size
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;	//purpose for use
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;		//means we cant get this from other queuefamilies


		if (vkCreateBuffer(mDevice, &bufferInfo, nullptr, &mVertexBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vertex buffer");
		}

		//we have to know what mem type for GPU so this is the setup for that all
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(mDevice, mVertexBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &mVertexBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate vertex buffer memory");
		}

		vkBindBufferMemory(mDevice, mVertexBuffer, mVertexBufferMemory, 0);
		*/

		void* data;

		//mapping the buffer memory into CPU accessible memory
		vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);	//doesnt immdeiately copy into buffer mem
		memcpy(data, object.mModel.msVertices.data(), (size_t)bufferSize);	//We can do this from the above HOST_COHERENT_BIT
		vkUnmapMemory(mDevice, stagingBufferMemory);

		createBuffer(mDevice, mPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, object.mModel.msVertexBuffer, object.mModel.msVertexBufferMemory);
		/*
			SRC_BIT: Buffer can be used as source in a mem transfer op
			DST_BIT: Buffer can be used as destination in mem transfer op
		*/

		copyBuffer(mDevice, mCommandPool, mGraphicsQueue, stagingBuffer, object.mModel.msVertexBuffer, bufferSize);	//move vertex data to device local buffer
		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);		//free out our staging buffer
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
	}
}


void WavefrontFileLoader::createIndexBuffer()
{
	for (auto& object : mOriginals)
	{
		VkDeviceSize bufferSize = sizeof(object.mModel.msIndices[0]) * object.mModel.msIndices.size();

		//actual staging buffer that only host visible buffer as temp buffer (then we later use device local as actual)
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(mDevice, mPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);


		void* data;

		//mapping the buffer memory into CPU accessible memory
		vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);	//doesnt immdeiately copy into buffer mem
		memcpy(data, object.mModel.msIndices.data(), (size_t)bufferSize);	//We can do this from the above HOST_COHERENT_BIT
		vkUnmapMemory(mDevice, stagingBufferMemory);

		createBuffer(mDevice, mPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, object.mModel.msIndexBuffer, object.mModel.msIndexBufferMemory);
		/*
			SRC_BIT: Buffer can be used as source in a mem transfer op
			DST_BIT: Buffer can be used as destination in mem transfer op
		*/

		copyBuffer(mDevice, mCommandPool, mGraphicsQueue, stagingBuffer, object.mModel.msIndexBuffer, bufferSize);	//move vertex data to device local buffer
		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);		//free out our staging buffer
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
	}
}


void WavefrontFileLoader::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t textureWidth, int32_t textureHeight, uint32_t mipLevels)
{
	//check if image format can actually support linear blitting
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		throw std::runtime_error("texture image format does not support linear blitting");
	}

	VkCommandBuffer commandBuffer = beginSingleTimeCommands(mDevice, mCommandPool);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = textureWidth;
	int32_t mipHeight = textureHeight;

	for (uint32_t i = 1; i < mipLevels; ++i)
	{
		//same as how all barriers are set up
		//	remember, we are generating a 2D surface, all Z values are 1 and shit is linear

		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;	//waits for i - 1 to be filled
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		//our blit function
		VkImageBlit blit = {};
		blit.srcOffsets[0] = { 0, 0, 0 };	//3D region data blitted from
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;		//our source iter place
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };	//regions divided by two beause we are halving
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;			//our destination iter place
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(commandBuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,	//blitting between different levels of same image
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,	//	so we use same image for both
			1, &blit, VK_FILTER_LINEAR);	//same filter options as vksampler use linear to enable interpolation

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		//transition mip level i - 1 to shader read only optimal
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &barrier);

		//divide mip levels by 2
		//	also handle if image isnt square
		if (mipWidth > 1)
		{
			mipWidth /= 2;
		}

		if (mipHeight > 1)
		{
			mipHeight /= 2;
		}

	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr, 0, nullptr, 1, &barrier);

	endSingleTimeCommands(mDevice, commandBuffer, mGraphicsQueue, mCommandPool);
}




