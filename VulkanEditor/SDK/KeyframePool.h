#pragma once
#define GLM_FORCE_RADIANS	//makes it so shit like rotate uses radians instead of eulerAngles
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES		//This is for solving contiguous memory for you (may have problems with nested stuff)
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		//depth testing, configure matrix to range of 0 to 1 instead of -1 to 1
#define GLM_ENABLE_EXPERIMENTAL		
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>		//rotate, lookAt, perspective
#include <glm/gtc/constants.hpp>			//pi 

#include <math.h>
#include <string>
#include <vector>

struct KeyframeData
{
public:
	KeyframeData(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
	{
		mPos = pos;
		mRot = rot;
		mScale = scale;
	}

	KeyframeData(float Px, float Py, float Pz, float Rx, float Ry, float Rz, float Sx, float Sy, float Sz)
	{
		mPos.x = Px;
		mPos.y = Py;
		mPos.z = Pz;

		mRot.x = Rx;
		mRot.y = Ry;
		mRot.z = Rz;

		mScale.x = Sx;
		mScale.y = Sy;
		mScale.z = Sz;
	}

	KeyframeData()
	{
		mPos.x = 0;
		mPos.y = 0;
		mPos.z = 0;

		mRot.x = 0;
		mRot.y = 0;
		mRot.z = 0;

		mScale.x = 1;
		mScale.y = 1;
		mScale.z = 1;
	}

	void setData(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
	{
		mPos = pos;
		mRot = rot;
		mScale = scale;
	}

	std::string outputPositionString()
	{
		std::string temp = "Position: ";
		temp += std::to_string(mPos.x);
		temp += " x, ";
		temp += std::to_string(mPos.y);
		temp += " y, ";
		temp += std::to_string(mPos.z);
		temp += " z";
		return temp;
	}

	std::string outputRotationString()
	{
		std::string temp = "Rotation: ";
		temp += std::to_string(mRot.x);
		temp += " x, ";
		temp += std::to_string(mRot.y);
		temp += " y, ";
		temp += std::to_string(mRot.z);
		temp += " z";
		return temp;
	}

	std::string outputScaleString()
	{
		std::string temp = "Scale: ";
		temp += std::to_string(mScale.x);
		temp += " x, ";
		temp += std::to_string(mScale.y);
		temp += " y, ";
		temp += std::to_string(mScale.z);
		temp += " z";
		return temp;
	}

	glm::vec3 mPos;
	glm::vec3 mRot;
	glm::vec3 mScale;
};

struct Keyframe
{

	Keyframe()		//default
	{
		mIndex = 0;
		mDuration = 0.0;
		mInvDuration = 0.0;
		mData.setData(glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0));
	}

	Keyframe(int index, float duration, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
	{
		mIndex = index;
		mDuration = duration;
		mInvDuration = reciprocal(duration);
		mData.setData(pos, rot, scale);
	}
	
	Keyframe(int index, float duration, KeyframeData data)
	{
		mIndex = index;
		mDuration = duration;
		mInvDuration = reciprocal(duration);
		mData = data;
	}

	~Keyframe() {};


	void setDuration(float duration)
	{
		mDuration = duration;
		mInvDuration = reciprocal(duration);
	}
	void setData(KeyframeData data) { mData = data; }
	void setIndex(int index) { mIndex = index; }

	float reciprocal(float duration) { return 1.0 / duration; }

	int mIndex;			//index in pool of keyframes
	float mDuration;	//interval of time for which this keyframe is active
	float mInvDuration;	//reciprocal of duration;

	KeyframeData mData;		//data held by keyframe (in this case, the 1D position of a channel of xyz)
};

class KeyframePool
{
public:
	KeyframePool()
	{
		mKeyframeCount = 0;
	}

	~KeyframePool() {}	//may needto deallocate the pool

	Keyframe getKeyframe(int index) { return mPool.at(index); }

	int size() { return mKeyframeCount; }

	void addKeyframe(Keyframe kf) 
	{ 
		++mKeyframeCount;
		mPool.push_back(kf); 
	}


private:
	std::vector<Keyframe> mPool;						//unordered, unsorted collection of keyframes
	int mKeyframeCount;											//number of keyframes in the pool

};