#pragma once

#include "UBOs.h"


/*
	Git is a stupid fucking piece of fucking shit
	i took extensive fucking notes on this fucking section and they all fucking vanished because
	git couldnt handle a fucking file over 100 mbs so i had to do a revert on the commit tree to update
	lfs and then ALL THIS FUCKING SHIT GOT LOST. fuck git. i learned this.
*/
struct Vertex {
	glm::vec3 pos;				//v (xyz)
	glm::vec3 color;
	glm::vec2 textureCoord;		//vt (uv)
	glm::vec3 normal;			//vn s

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		/*
		float: VK_FORMAT_R32_SFLOAT
		vec2: VK_FORMAT_R32G32_SFLOAT
		vec3: VK_FORMAT_R32G32B32_SFLOAT
		vec4: VK_FORMAT_R32G32B32A32_SFLOAT
		*/

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, textureCoord);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, normal);

		return attributeDescriptions;
	}

	//use for unordered map, loading models (equality test and hash calculation)
	bool operator==(const Vertex& rhs) const {
		return pos == rhs.pos && color == rhs.color && textureCoord == rhs.textureCoord && rhs.normal == normal;
		//return pos == rhs.pos && textureCoord == rhs.textureCoord;
	}
};

//this is a hash function that combines fields of a struct (in this case vertex) to create 
namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			//return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
			//	(hash<glm::vec2>()(vertex.textureCoord) << 1);

			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) ^ (hash<glm::vec2>()(vertex.normal)) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.textureCoord) << 1);

			//return (hash<glm::vec3>()(vertex.pos) >> 1 ) ^ (hash<glm::vec2>()(vertex.textureCoord) << 1);
		}
	};

}


struct sourced3D
{
	UniformBufferObject msUBO = {};

	std::vector<VkDescriptorSet> msDescriptorSets;

	std::vector<VkBuffer> msUniformBuffers;
	std::vector<VkDeviceMemory> msUniformBuffersMemory;
	uint32_t msMipLevels;
	VkImage msTextureImage;
	VkDeviceMemory msTextureImageMemory;
	VkImageView msTextureImageView;
	VkSampler msTextureSampler;

	std::string msTexturePath;
	std::string msModelPath;

	std::string msVertShaderPath = "../shaders/vert.spv";
	std::string msFragShaderPath = "../shaders/frag.spv";
	VkPipelineShaderStageCreateInfo msShaderStages;	//this may need a number in it
};


struct light3D
{
	glm::vec3 lightPos;
	glm::vec4 lightColor;
	glm::float32 lightIntensity; 
	glm::float32 lightSize;
};


