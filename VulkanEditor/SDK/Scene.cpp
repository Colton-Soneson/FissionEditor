#include "Scene.h"

void Scene::loadScene()
{

}

void Scene::storeObject(sourced3D obj)
{
	mSceneContent.push_back(obj);

	mSCF.mSceneModelsModel.push_back(obj.msModelPath);
	mSCF.mSceneModelsTexture.push_back(obj.msTexturePath);
	mSCF.mSceneModelsMat.push_back(obj.msUBO.model);
}

void Scene::storeObject(std::string texturePath, std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{

}

void Scene::storeLight(light3D light)
{
	/*mLightSources.mLightPositions.push_back(light.lightPos);
	mLightSources.mLightColors.push_back(light.lightColor);
	mLightSources.mLightIntensities.push_back(light.lightIntensity);
	mLightSources.mLightSizes.push_back(light.lightSize);
	mLightSources.mLightCount++;*/

	mLightSources.push_back(light);
}

void Scene::storeLight(glm::vec3 lightPos, glm::vec4 lightColor, glm::float32 lightIntensity, glm::float32 lightSize)
{
	/*mLightSources.mLightPositions.push_back(lightPos);
	mLightSources.mLightColors.push_back(lightColor);
	mLightSources.mLightIntensities.push_back(lightIntensity);
	mLightSources.mLightSizes.push_back(lightSize);
	mLightSources.mLightCount++;*/

	light3D light;
	light.lightPos = lightPos;
	light.lightColor = lightColor;
	light.lightIntensity = lightIntensity;
	light.lightSize = lightSize;

	mLightSources.push_back(light);
}