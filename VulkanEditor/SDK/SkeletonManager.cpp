#include "SkeletonManager.h"

void SkeletonManager::update()
{
	//only for humanoid basics
	for (HumanoidBasic &hb : mSkeletonContainer.mHumanoidBasics)
	{
		hb.calculateGlobalSpacePose();
	}
}

void SkeletonManager::createHumanoidBasic()
{
	HumanoidBasic temp;
	temp.createBasePose();
	mSkeletonContainer.mHumanoidBasics.push_back(temp);
}