#pragma once
#include "UniversalObjectStorage.h"
#include "SkeletonManager.h"

#include <fstream>

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
		mpObjectKeyframePool = new KeyframeObjectPool();
		mpObjectClipPool = new ClipObjectPool();
		mpSkeletonManager = new SkeletonManager();
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
	int getIndexFromObjectName(std::string name);

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

	//--------------------ANIMATION---------------------
	
	//OBJECT BASED
	void addObjectClipController(std::string name);
	std::vector<ClipControllerObject*> getObjectClipControllers() { return mObjectClipControllers; };
	ClipControllerObject* getObjectClipControllerByName(std::string name);

	void addKeyframeToObjectKeyframePool(KeyframeObject kf);
	void addKeyframeToObjectKeyframePool(int index, float duration, KeyframeObjectData data);
	void addKeyframeToObjectKeyframePool(int index, float duration, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
	KeyframeObjectPool* getObjectKeyframePool() { return mpObjectKeyframePool; }

	void addClipToObjectClipPool(ClipObject c);
	void addClipToObjectClipPool(int firstKeyframe, int lastKeyframe);
	void addClipToObjectClipPool(int firstKeyframe, int lastKeyframe, float fixedDuration);
	ClipObjectPool* getObjectClipPool() { return mpObjectClipPool; }


	//SKELETAL BASED
	void addSkeletalClipController(std::string name);
	std::vector<ClipControllerSkeletal*> getSkeletalClipControllers() { return mSkeletalClipControllers; };
	ClipControllerSkeletal* getSkeletalClipControllerByName(std::string name);

	void addKeyframeToSkeletalKeyframePool(KeyframeSkeletal kf);
	void addKeyframeToSkeletalKeyframePool(int index, float duration, KeyframeSkeletalData data);
	KeyframeSkeletalPool* getSkeletalKeyframePool() { return mpSkeletalKeyframePool; }

	void addClipToSkeletalClipPool(ClipSkeletal c);
	void addClipToSkeletalClipPool(int firstKeyframe, int lastKeyframe);
	void addClipToSkeletalClipPool(int firstKeyframe, int lastKeyframe, float fixedDuration);
	ClipSkeletalPool* getSkeletalClipPool() { return mpSkeletalClipPool; }


	SkeletonManager* getSkeletonManager() { return mpSkeletonManager; }

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
	std::vector<ClipControllerObject*> mObjectClipControllers;
	std::vector<ClipControllerSkeletal*> mSkeletalClipControllers;
	ClipObjectPool* mpObjectClipPool;
	ClipSkeletalPool* mpSkeletalClipPool;
	KeyframeObjectPool* mpObjectKeyframePool;
	KeyframeSkeletalPool* mpSkeletalKeyframePool;
	SkeletonManager* mpSkeletonManager;
	float mEngineTimeStep;

};