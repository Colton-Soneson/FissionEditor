#pragma once
#include "ModelData.h"

#include <fstream>
#include <vector>

struct SceneContentFile
{
	std::vector<std::string> mSceneModelsTexture;
	std::vector<std::string> mSceneModelsModel;
	std::vector<glm::mat4> mSceneModelsMat;
};


class Scene
{
	friend class DemoApplication;
public:
	Scene(std::string filename)
	{
		mFilename = filename;	//optional file save name
	};

	Scene() { mFilename = ""; };

	~Scene() {};

	std::vector<sourced3D> &getObjects() { return mSceneContent; };
	std::vector<light3D> &getLights() { return mLightSources; };
	sourced3D& getSkybox() { return mSkyBoxObject; };

	//objects
	void storeObject(sourced3D obj, std::string name);
	void storeObject(std::string texturePath, std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, std::string name);	//call this when creating new object in GUI

	//lighting
	void storeLight(light3D light, std::string name);
	void storeLight(glm::vec3 lightPos, glm::vec4 lightColor, glm::float32 lightIntensity, glm::float32 lightSize, std::string name);

	//misc
	void storeSkyboxFromCube(sourced3D skybox) { mSkyBoxObject = skybox; };		//WE NEED TO LOAD THIS WITH THE NORMALS SET AS NEGATIVE
	void storeSkyboxFromCubemap(sourced3D skybox) { mSkyBoxObject = skybox; };		//This is the 6 seperate planes
	
	//file reading
	void loadScene();
	
private:

	std::vector<sourced3D> mSceneContent;
	std::string mFilename;

	std::vector<light3D> mLightSources;

	sourced3D mSkyBoxObject;

	SceneContentFile mSCF;
};