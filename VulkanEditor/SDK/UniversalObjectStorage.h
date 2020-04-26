#pragma once

#include "WavefrontFileLoader.h"
#include "ProceduralLoader.h"

class UniversalObjectStorage
{
public:

	UniversalObjectStorage(std::string filepath) 
	{
		mFilePath = filepath;
		mWFL = new WavefrontFileLoader(mFilePath);
		mPL = new ProceduralLoader();
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
	ProceduralLoader* mPL;

};