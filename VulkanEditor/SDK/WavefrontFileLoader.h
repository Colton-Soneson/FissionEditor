#pragma once

#include <fstream>
#include <vector>


#include "ModelData.h"


class WavefrontFileLoader
{
public:

	WavefrontFileLoader(std::string filepath)
	{
		mFilePath = filepath;
	};
	
	~WavefrontFileLoader() {};


	void startLoadingFromFile();

	std::vector<sourced3D> getLoadedObjects();


private:

	std::vector<sourced3D> mOriginals;

	std::string mFilePath;
};