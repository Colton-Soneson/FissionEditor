#include "UniversalObjectStorage.h"

void UniversalObjectStorage::runInitialObjectStorage()
{
	//do wavefront models
	mWFL->startLoadingFromFile();

	for (auto& object : mWFL->getLoadedObjects())
	{
		mTotalOriginals.push_back(object);
	}

	//do fbx models

}

//std::vector<std::string> UniversalObjectStorage::getTotalLoadedObjectsNames()
//{
//
//}

std::vector<sourced3D> UniversalObjectStorage::getTotalLoadedObjects()
{
	if (mTotalOriginals.empty())
	{
		std::cout << "\nWARNING:  (UniversalObjectStorage) OriginalsList is empty" << "\n";
	}
	
	return mTotalOriginals;
}


void UniversalObjectStorage::createProceduralModel(ShapeData type, float params[])
{
	/*
	switch (type)
	{
	case PLANE:
		if (params[0] != NULL && params[1] != NULL && params[2] != NULL)
		{
			mTotalOriginals.push_back(mPL->createAndStorePlane(params[0], params[1], params[2]));
		}
		else
		{
			throw std::runtime_error("(UniversalObjectStorage) parameters for Plane object insufficient");
		}
		break;

	case CUBE:
		if (params[0] != NULL && params[1] != NULL && params[2] != NULL && params[3] != NULL)
		{
			mTotalOriginals.push_back(mPL->createAndStoreCube(params[0], params[1], params[2], params[3]));
		}
		else
		{
			throw std::runtime_error("(UniversalObjectStorage) parameters for Cube object insufficient");
		}
		break;

	case ICOSPHERE:
		if (params[0] != NULL && params[1] != NULL)
		{
			mTotalOriginals.push_back(mPL->createAndStoreIcoSphere(params[0], params[1]));
		}
		else
		{
			throw std::runtime_error("(UniversalObjectStorage) parameters for IcoSphere object insufficient");
		}
		break;

	case UVSPHERE:
		if (params[0] != NULL && params[1] != NULL && params[2] != NULL)
		{
			mTotalOriginals.push_back(mPL->createAndStoreUVSphere(params[0], params[1], params[2]));
		}
		else
		{
			throw std::runtime_error("(UniversalObjectStorage) parameters for UVSphere object insufficient");
		}
		break;

	default:
		throw std::runtime_error("UniversalObjectStorage) ShapeData type unavailable");
		break;
	}
	*/
}


