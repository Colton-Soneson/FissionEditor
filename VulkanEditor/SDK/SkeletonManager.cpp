#include "SkeletonManager.h"

void SkeletonManager::update()
{
	//only for humanoid basics
	for (HumanoidBasic &hb : mSkeletonContainer.mHumanoidBasics)
	{
		//interpolation
		//concatenation
		//conversion
		//FK

		
	}
}

void SkeletonManager::createHumanoidBasic()
{
	HumanoidBasic temp;
	temp.createBasePose();
	mSkeletonContainer.mHumanoidBasics.push_back(temp);
}