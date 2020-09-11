#pragma once
#define GLM_FORCE_RADIANS	//makes it so shit like rotate uses radians instead of eulerAngles
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES		//This is for solving contiguous memory for you (may have problems with nested stuff)
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		//depth testing, configure matrix to range of 0 to 1 instead of -1 to 1
#define GLM_ENABLE_EXPERIMENTAL		
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>		//rotate, lookAt, perspective
#include <glm/gtc/constants.hpp>			//pi 

#include <math.h>
#include <vector>

struct Keyframe
{

	Keyframe()		//default
	{
		mIndex = 0;
		mDuration = 0.0;
		mInvDuration = 0.0;
		mData = 0;
	}

	Keyframe(int index, float duration, float data)
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
	void setData(float data) { mData = data; }
	void setIndex(int index) { mIndex = index; }

	float reciprocal(float duration) { return 1.0 / duration; }

	int mIndex;			//index in pool of keyframes
	float mDuration;	//interval of time for which this keyframe is active
	float mInvDuration;	//reciprocal of duration;

	float mData;		//data held by keyframe (in this case, the 1D position of a channel of xyz)
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