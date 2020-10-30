#pragma once
#include "KeyframePool.h"
#include <string>



struct Clip
{
	Clip(KeyframePool* kfpool, int firstKeyframe, int lastKeyframe)
	{
		mDuration = 0.0;
		mInvDuration = 0.0;
		mClipKeyframeCount = 0;
		mKeyframePool = kfpool;
		mFirstKeyframeIndex = firstKeyframe;
		mLastKeyframeIndex = lastKeyframe;
		
		//duration using the keyframe times themselves
		calculateDuration();
	}

	Clip(KeyframePool* kfpool, int firstKeyframe, int lastKeyframe, float fixedDuration)
	{
		mDuration = 0.0;
		mInvDuration = 0.0;
		mClipKeyframeCount = 0;
		mKeyframePool = kfpool;
		mFirstKeyframeIndex = firstKeyframe;
		mLastKeyframeIndex = lastKeyframe;
		mTransitionalForward = false;
		mTransitionalBackward = false;

		//duration using set fixed duration distributed amount keyframes
		distributeDuration(fixedDuration);
	}

	void calculateDuration()	//calculate the duration and inverse as the sum of all referenced keyframes
	{
		for (int i = mFirstKeyframeIndex; i <= mLastKeyframeIndex; ++i)
		{
			++mClipKeyframeCount;
			mDuration += mKeyframePool->getKeyframe(i).mDuration;
			mInvDuration += mKeyframePool->getKeyframe(i).mInvDuration;
		}
	}

	void distributeDuration(float fixedDuration)	//set duration and inverse directly and distribute it across all referenced keyframes to give them all uniform duration
	{
		for (int i = mFirstKeyframeIndex; i <= mLastKeyframeIndex; ++i)
		{
			++mClipKeyframeCount;

			mKeyframePool->getKeyframe(i).setDuration(fixedDuration);

			mDuration += mKeyframePool->getKeyframe(i).mDuration;
			mInvDuration += mKeyframePool->getKeyframe(i).mInvDuration;
		}
	}

	float reciprocal(float duration) { return 1.0 / duration; }

	std::string mName;
	int mIndex;					//clip index inside the clip pool
	float mDuration;			//can be calculated as sum of all the referenced keyframes or set first and distributed uniformly across keyframes
	float mInvDuration;			//reciprocal of duration
	int mClipKeyframeCount;			//number of keyframes referenced by clip, including first and last

	KeyframePool* mKeyframePool;//in the array, the clip will be the sequence of keyframes from FIRST to LAST. This is where decoupling and abstraction is used
								//		A CLIP DOESNT OWN KEYFRAMES, it just references a set of some that exist someplace else
	int mFirstKeyframeIndex;	//index of first keyframe in keyframe pool referenced by clip
	int mLastKeyframeIndex;		//index of last keyframe in keyframe pool referenced by clip

	bool mTransitionalForward;
	bool mTransitionalBackward;
};

struct ClipPool
{
	ClipPool()
	{
		mClipCount = 0;
	}

	~ClipPool()
	{
		//deallocate the pool
	}

	void addClip(Clip c)
	{
		++mClipCount;
		mClips.push_back(c);
	}

	int getClipByName(std::string name)		//search for the clip by name, return its index
	{
		for (Clip& itr : mClips)
		{
			if (itr.mName.compare(name) == 0)
			{
				return itr.mIndex;
				break;
			}
		}
	}

	std::vector<Clip> getClips() { return mClips; }
	


	int mClipCount;
	std::vector<Clip> mClips;
};

struct Transitionals
{
public:

	Transitionals()
	{
		mTargetClipIndex = 0;
		mClipPool = nullptr;
	}

	Transitionals(ClipPool* cP)
	{
		mTargetClipIndex = 0;
		mClipPool = cP;
	}

	void replaceClip(int& currentClipIndex, int& currentKeyframeIndex, int direction)
	{

		//if (mClipPool->getClips().at(currentClipIndex).mTransitionalBackward == true)
		if (direction == -1)
		{
			if ((currentClipIndex - 1) < 0)	//no more clips in pool to go through
			{
				currentClipIndex = mClipPool->getClips().size() - 1;
				currentKeyframeIndex = mClipPool->getClips().at(currentClipIndex).mLastKeyframeIndex;
			}
			else
			{
				--currentClipIndex;
				currentKeyframeIndex = mClipPool->getClips().at(currentClipIndex).mLastKeyframeIndex;
			}

		}

		//if (mClipPool->getClips().at(currentClipIndex).mTransitionalForward == true)
		if (direction == 1)
		{
			if ((currentClipIndex + 1) > mClipPool->getClips().size() - 1)
			{
				currentClipIndex = 0;
				currentKeyframeIndex = mClipPool->getClips().at(currentClipIndex).mFirstKeyframeIndex;
			}
			else
			{
				++currentClipIndex;
				currentKeyframeIndex = mClipPool->getClips().at(currentClipIndex).mFirstKeyframeIndex;
			}
		}
	}

	void setClipPool(ClipPool* cP) { mClipPool = cP; }
	void setTargetIndex(int tCI) { mTargetClipIndex = tCI; }


	int mTargetClipIndex;	//this will be either fed with a parameter of clipIndex + 1 or -1 depending for
							//		forward or backward
	ClipPool* mClipPool;
};

class ClipController
{
public:
	
	ClipController(std::string name, ClipPool* pool)
	{
		mName = name;
		mpClipPool = pool;
		mClipIndex = -1;	//if -1 then we dont have to clip to control yet so skip in the update
		mKeyframeIndex = 0;
		mSlowMoMultiplier = 1;
		mClipTime = 0.0;
		mKeyframeTime = 0.0;
		mTransitionalClipMode = false;
		mTransitionalManager = new Transitionals(pool);
	}

	void update(float dt);

	void setClipPool(ClipPool* pool) { mpClipPool = pool; }
	void setClipToUseByIndex(int cpIndex) { mClipIndex = cpIndex; }
	void setStartToLastKeyframe() { mKeyframeIndex = mpClipPool->getClips().at(mClipIndex).mLastKeyframeIndex; }
	void setStartToFirstKeyframe() {mKeyframeIndex = mpClipPool->getClips().at(mClipIndex).mFirstKeyframeIndex; }

	float& getSlowMoMultiplier() { return mSlowMoMultiplier; }

	int getCurrentKeyframeIndex() { return mKeyframeIndex; }
	int getClipIndexInPool() { return mClipIndex; }	//returns -1 when empty

	int& getPlaybackDirection() { return mPlaybackDirection; } //can be adjusted from ImGUI interface this way

	void setTransitionalMode(bool mode) { mTransitionalClipMode = mode; }

	void lerpUpdate(float dt, glm::vec3& pos, glm::vec3& rot, glm::vec3& scale);		//lerping for this specific controller
	

	std::string getName() { return mName; }

private:
	std::string mName;			//the clipcontroller "X" controls clip "Y", dont name them the same
	int mClipIndex;				//index of clip to control in referenced clip pool (tells which clip is being played back by the controller)
	float mClipTime;			//current time relative to start of clip (between 0 and clip's duration)
								//		one exception makes it [0, duration)
	float mClipParameter;		//normalized keyframe time between 0 and 1
								//		one exception makes it [0, 1)
	int mKeyframeIndex;			//index of current keyframe in referenced keyfame pool (tells of progress in the clip)
	float mKeyframeTime;		//current time relative to current keyframe, between 0 and 1
								//		one exception of [0, current keyframe duration)
	float mKeyframeParameter;	//normalized keyframe time between 0 and 1
								//		one exception makes it [0, 1)
	int mPlaybackDirection;		// +1 for forward, 0 for pause, and -1 for reverse

	float mSlowMoMultiplier;	//effect SloMo will have on timestep
	
	ClipPool* mpClipPool;					//reference to the pool of clips to control
	Transitionals* mTransitionalManager;
	bool mTransitionalClipMode;

};
