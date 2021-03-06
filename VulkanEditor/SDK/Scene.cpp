#include "Scene.h"



void Scene::instantiateObject(int objectListIndex)
{
	//make sure to reset the following calls in DemoApp to the sceneContent after this has been called
	mSceneContent.push_back(mUOS->getTotalLoadedObjects().at(objectListIndex));

	//the point of this to have unique names on objects
	std::stringstream temp;
	temp << mSceneContent.back().msName << mCount;
	mSceneContent.back().msName = temp.str();
	mCount++;
}

void Scene::instantiateObject(int objectListIndex, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, float ambientMod, bool activateLighting)
{
	sourced3D temp3D = mUOS->getTotalLoadedObjects().at(objectListIndex);

	temp3D.msUBO.model = glm::translate(glm::mat4(1.0f), position);

	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::degrees(rotation.x), glm::vec3(1, 0, 0));
	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::degrees(rotation.y), glm::vec3(0, 1, 0));
	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::degrees(rotation.z), glm::vec3(0, 0, 1));

	temp3D.msUBO.model = glm::scale(temp3D.msUBO.model, scale);
	temp3D.msUBO.ambientMod = ambientMod;

	if (activateLighting == true)
	{
		temp3D.msUBO.activeLight = 1;
	}
	else
	{
		temp3D.msUBO.activeLight = 0;
	}
	
	mSceneContent.push_back(temp3D);

	std::stringstream temp;
	temp << mSceneContent.back().msName << mCount;
	mSceneContent.back().msName = temp.str();
	mCount++;

}

int Scene::getIndexFromObjectName(std::string name)
{
	int objIndex = 0;

	for (int i = 0; i < getUOSTotalStorageNumber(); i++)
	{
		std::string temp = getUOSNameByIndex(i);
		if (temp == name)
		{
			objIndex = i;
			std::cout << getUOSNameByIndex(i);
			break;
		}
	}

	return objIndex;
}

void Scene::adjustObject(int sceneContentIndex, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, float ambientMod, bool activateLighting)
{
	sourced3D temp3D = mSceneContent.at(sceneContentIndex);
	
	temp3D.msUBO.model = glm::translate(glm::mat4(1.0f), position);

	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::degrees(rotation.x), glm::vec3(1, 0, 0));
	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::degrees(rotation.y), glm::vec3(0, 1, 0));
	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::degrees(rotation.z), glm::vec3(0, 0, 1));

	temp3D.msUBO.model = glm::scale(temp3D.msUBO.model, scale);
	temp3D.msUBO.ambientMod = ambientMod;

	if (activateLighting == true)
	{
		temp3D.msUBO.activeLight = 1;
	}
	else
	{
		temp3D.msUBO.activeLight = 0;
	}


	std::vector<sourced3D>::iterator sci = mSceneContent.begin() + sceneContentIndex;
	mSceneContent.erase(sci);
	sci = mSceneContent.begin() + (sceneContentIndex);
	mSceneContent.insert(sci, temp3D);
}

void Scene::removeObject(int sceneContentIndex)
{
	mSceneContent.erase(mSceneContent.begin() + sceneContentIndex);
}

void Scene::storeLight(light3D light, std::string name)
{
	//mLightSources.mLightPositions.push_back(light.lightPos);
	//mLightSources.mLightColors.push_back(light.lightColor);
	//mLightSources.mLightIntensities.push_back(light.lightIntensity);
	//mLightSources.mLightSizes.push_back(light.lightSize);
	//mLightSources.mLightCount++;
	light.setName(name);
	mLightSources.push_back(light);
}

void Scene::storeLight(glm::vec3 lightPos, glm::vec4 lightColor, glm::float32 lightIntensity, glm::float32 lightSize, std::string name)
{
	//mLightSources.mLightPositions.push_back(lightPos);
	//mLightSources.mLightColors.push_back(lightColor);
	//mLightSources.mLightIntensities.push_back(lightIntensity);
	//mLightSources.mLightSizes.push_back(lightSize);
	//mLightSources.mLightCount++;

	light3D light;
	light.lightPos = lightPos;
	light.lightColor = lightColor;
	light.lightIntensity = lightIntensity;
	light.lightSize = lightSize;

	mLightSources.push_back(light);
}

void Scene::addObjectClipController(std::string name)
{
	ClipControllerObject* temp = new ClipControllerObject(name, mpObjectClipPool);
	mObjectClipControllers.push_back(temp);
}

ClipControllerObject* Scene::getObjectClipControllerByName(std::string name)
{
	for (ClipControllerObject* CC : mObjectClipControllers)
	{
		if (CC->getName().compare(name) == 0)
		{
			return CC;
		}
	}

	return nullptr;
}

void Scene::addKeyframeToObjectKeyframePool(KeyframeObject kf)
{
	mpObjectKeyframePool->addKeyframe(kf);
}

void Scene::addKeyframeToObjectKeyframePool(int index, float duration, KeyframeObjectData data)
{
	KeyframeObject temp(index, duration, data);
	mpObjectKeyframePool->addKeyframe(temp);
}

void Scene::addKeyframeToObjectKeyframePool(int index, float duration, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	KeyframeObject temp(index, duration, pos, rot, scale);
	mpObjectKeyframePool->addKeyframe(temp);
}

void Scene::addClipToObjectClipPool(ClipObject c)
{
	mpObjectClipPool->addClip(c);
}

void Scene::addClipToObjectClipPool(int firstKeyframe, int lastKeyframe)
{
	ClipObject temp(mpObjectKeyframePool, firstKeyframe, lastKeyframe);
	mpObjectClipPool->addClip(temp);
}

void Scene::addClipToObjectClipPool(int firstKeyframe, int lastKeyframe, float fixedDuration)
{
	ClipObject temp(mpObjectKeyframePool, firstKeyframe, lastKeyframe, fixedDuration);
	mpObjectClipPool->addClip(temp);
}

void Scene::addSkeletalClipController(std::string name)
{
	ClipControllerSkeletal* temp = new ClipControllerSkeletal(name, mpSkeletalClipPool);
	mSkeletalClipControllers.push_back(temp);
}

ClipControllerSkeletal* Scene::getSkeletalClipControllerByName(std::string name)
{
	for (ClipControllerSkeletal* CC : mSkeletalClipControllers)
	{
		if (CC->getName().compare(name) == 0)
		{
			return CC;
		}
	}

	return nullptr;
}

void Scene::addKeyframeToSkeletalKeyframePool(KeyframeSkeletal kf)
{
	mpSkeletalKeyframePool->addKeyframe(kf);
}

void Scene::addKeyframeToSkeletalKeyframePool(int index, float duration, KeyframeSkeletalData data)
{
	KeyframeSkeletal temp(index, duration, data);
	mpSkeletalKeyframePool->addKeyframe(temp);
}

void Scene::addClipToSkeletalClipPool(ClipSkeletal c)
{
	mpSkeletalClipPool->addClip(c);
}

void Scene::addClipToSkeletalClipPool(int firstKeyframe, int lastKeyframe)
{
	ClipSkeletal temp(mpSkeletalKeyframePool, firstKeyframe, lastKeyframe);
	mpSkeletalClipPool->addClip(temp);
}

void Scene::addClipToSkeletalClipPool(int firstKeyframe, int lastKeyframe, float fixedDuration)
{
	ClipSkeletal temp(mpSkeletalKeyframePool, firstKeyframe, lastKeyframe, fixedDuration);
	mpSkeletalClipPool->addClip(temp);
}

