#include "WavefrontFileLoader.h"

std::vector<sourced3D> WavefrontFileLoader::getLoadedObjects()
{
	//check if we dont have any objects in the array, then just output a warning
	
	if (mOriginals.empty())
	{
		std::cout << "\nWARNING: (WavefrontFileLoader) no obj files present in the directory: " << mFilePath << "\n";
	}
	else
	{
		return mOriginals;
	}
}

void WavefrontFileLoader::startLoadingFromFile()
{

}

