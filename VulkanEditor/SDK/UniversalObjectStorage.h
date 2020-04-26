#pragma once

#include "WavefrontFileLoader.h"
//#include "ProceduralLoader.h"

class UniversalObjectStorage
{
public:

	UniversalObjectStorage(std::string filepath, VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool)
	{
		mFilePath = filepath;
		mWFL = new WavefrontFileLoader(mFilePath, device, physicalDevice, graphicsQueue, commandPool);
		//mPL = new ProceduralLoader();
	};

	~UniversalObjectStorage() {};

	void runInitialObjectStorage();
	//void objectAddedUpdate();		//fill the objects in their respective loaders

	void createProceduralModel(ShapeData type, float params[]);
	//ProceduralLoader* getProceduralLoader() { return mPL; };

	//std::vector<std::string> getTotalLoadedObjectsNames();
	std::vector<sourced3D> getTotalLoadedObjects();

private:

	std::vector<sourced3D> mTotalOriginals;

	std::string mFilePath;

	WavefrontFileLoader* mWFL;
	//ProceduralLoader* mPL;

};