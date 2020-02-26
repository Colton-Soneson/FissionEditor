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

	Scene() {};

	~Scene() {};

	std::vector<sourced3D> &getObjects() { return mSceneContent; };
	std::vector<light3D> &getLights() { return mLightSources; };

	void storeObject(sourced3D obj);
	void storeObject(std::string texturePath, std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);	//call this when creating new object in GUI
	void storeLight(light3D light);
	void storeLight(glm::vec3 lightPos, glm::vec4 lightColor, glm::float32 lightIntensity, glm::float32 lightSize);
	
	void loadScene();
	
private:

	std::vector<sourced3D> mSceneContent;
	std::string mFilename;

	std::vector<light3D> mLightSources;

	SceneContentFile mSCF;
};