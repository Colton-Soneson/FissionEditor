#pragma once
#include "UniversalObjectStorage.h"
#include "ClipController.h"

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
	Scene(std::string modelFilepath, std::string textureFilepath, VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool)
	{
		mModelFilePath = modelFilepath;	
		mTextureFilePath = textureFilepath;	
		mCount = 0;
		mUOS = new UniversalObjectStorage(mModelFilePath, mTextureFilePath, device, physicalDevice, graphicsQueue, commandPool);
		mpKeyframePool = new KeyframePool();
		mpClipPool = new ClipPool();
	};

	~Scene() {};

	std::vector<sourced3D> &getObjects() { return mSceneContent; };
	std::vector<light3D> &getLights() { return mLightSources; };
	sourced3D& getSkybox() { return mSkyBoxObject; };
	
	//inital full directory load
	void runDirectoryLoad() { mUOS->runInitialObjectStorage(); };
	int getUOSTotalStorageNumber() { return mUOS->getTotalLoadedObjects().size(); };		//total number of objects stored
	std::string getUOSNameByIndex(int index) { return mUOS->getNameAtIndex(index); };
	UniversalObjectStorage* getUOS() { return mUOS; };

	////objects
	void instantiateObject(int objectListIndex);
	void instantiateObject(int objectListIndex, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, float ambientMod, bool activateLighting);

	void adjustObject(int sceneContentIndex, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, float ambientMod, bool activateLighting);
	//void storeObject(sourced3D obj, std::string name);
	//void storeObject(std::string texturePath, std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, std::string name);	//call this when creating new object in GUI

	//lighting
	void storeLight(light3D light, std::string name);
	void storeLight(glm::vec3 lightPos, glm::vec4 lightColor, glm::float32 lightIntensity, glm::float32 lightSize, std::string name);

	////misc
	//void storeSkyboxFromCube(sourced3D skybox) { mSkyBoxObject = skybox; };		//WE NEED TO LOAD THIS WITH THE NORMALS SET AS NEGATIVE
	//void storeSkyboxFromCubemap(sourced3D skybox) { mSkyBoxObject = skybox; };		//This is the 6 seperate planes
	//
	////file reading
	//void loadScene();

	void setEngineTimeStep(float dt) { mEngineTimeStep = dt; }

	//animation
	void addClipController(std::string name);
	std::vector<ClipController*> getClipControllers() { return mClipControllers; };
	ClipController* getClipControllerByName(std::string name);

	void addKeyframeToKeyframePool(Keyframe kf);
	void addKeyframeToKeyframePool(int index, float duration, KeyframeData data);
	void addKeyframeToKeyframePool(int index, float duration, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
	KeyframePool* getKeyframePool() { return mpKeyframePool; }

	void addClipToClipPool(Clip c);
	void addClipToClipPool(int firstKeyframe, int lastKeyframe);
	void addClipToClipPool(int firstKeyframe, int lastKeyframe, float fixedDuration);
	ClipPool* getClipPool() { return mpClipPool; }

	float getEngineTimeStep() { return mEngineTimeStep; }

	
private:

	UniversalObjectStorage* mUOS;

	std::vector<sourced3D> mSceneContent;

	std::string mModelFilePath;
	std::string mTextureFilePath;

	std::vector<light3D> mLightSources;

	sourced3D mSkyBoxObject;

	SceneContentFile mSCF;

	int mCount;

	//animation
	std::vector<ClipController*> mClipControllers;
	ClipPool* mpClipPool;
	KeyframePool* mpKeyframePool;
	float mEngineTimeStep;

};