#include "Scene.h"




//LEGACY CRAP
/*
void Scene::loadScene()
{

}

void Scene::storeObject(sourced3D obj, std::string name)
{
	obj.setName(name);
	mSceneContent.push_back(obj);

	mSCF.mSceneModelsModel.push_back(obj.msModelPath);
	mSCF.mSceneModelsTexture.push_back(obj.msTexturePath);
	mSCF.mSceneModelsMat.push_back(obj.msUBO.model);
}

void Scene::storeObject(std::string texturePath, std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, std::string name)
{

}
*/


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

	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

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

	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	temp3D.msUBO.model = glm::rotate(temp3D.msUBO.model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

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