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


	mSceneContent.erase(mSceneContent.begin() + (sceneContentIndex));
	mSceneContent.push_back(temp3D);

	
	
	//mSceneContent.at(sceneContentIndex).msUBO.model = glm::scale(mSceneContent.at(sceneContentIndex).msUBO.model, scale);

	//mSceneContent.at(sceneContentIndex).msUBO.model = glm::rotate(mSceneContent.at(sceneContentIndex).msUBO.model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	//mSceneContent.at(sceneContentIndex).msUBO.model = glm::rotate(mSceneContent.at(sceneContentIndex).msUBO.model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	//mSceneContent.at(sceneContentIndex).msUBO.model = glm::rotate(mSceneContent.at(sceneContentIndex).msUBO.model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

	//mSceneContent.at(sceneContentIndex).msUBO.model = glm::scale(mSceneContent.at(sceneContentIndex).msUBO.model, scale);

	//mSceneContent.at(sceneContentIndex).msUBO.ambientMod = ambientMod;

	//if (activateLighting == true)
	//{
	//	mSceneContent.at(sceneContentIndex).msUBO.activeLight = 1;
	//}
	//else
	//{
	//	mSceneContent.at(sceneContentIndex).msUBO.activeLight = 0;
	//}
	
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

void Scene::addClipController(std::string name)
{
	ClipController* temp = new ClipController(name, mpClipPool);
	mClipControllers.push_back(temp);
}

ClipController* Scene::getClipControllerByName(std::string name)
{
	for (ClipController* CC : mClipControllers)
	{
		if (CC->getName().compare(name) == 0)
		{
			return CC;
		}
	}

	return nullptr;
}

void Scene::addKeyframeToKeyframePool(Keyframe kf)
{
	mpKeyframePool->addKeyframe(kf);
}

void Scene::addKeyframeToKeyframePool(int index, float duration, float data)
{
	Keyframe temp(index, duration, data);
	mpKeyframePool->addKeyframe(temp);
}

void Scene::addClipToClipPool(Clip c)
{
	mpClipPool->addClip(c);
}

void Scene::addClipToClipPool(int firstKeyframe, int lastKeyframe)
{
	Clip temp(mpKeyframePool, firstKeyframe, lastKeyframe);
	mpClipPool->addClip(temp);
}

void Scene::addClipToClipPool(int firstKeyframe, int lastKeyframe, float fixedDuration)
{
	Clip temp(mpKeyframePool, firstKeyframe, lastKeyframe, fixedDuration);
	mpClipPool->addClip(temp);
}


