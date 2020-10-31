#pragma once
#include "KeyframePool.h"
#include <string>

enum OpType
{
	OT_IDENTITY,
	OT_ADD,
	OT_SCALE,
	OT_MIX,
	OT_CUBIC,
	OT_BILERP,
	OT_BICUBIC
};

struct BlendNode
{
	
	BlendNode(OpType _op, std::vector<HierarchicalPose> _h, std::vector<float*> _u)
	{
		mType = _op;
		mHParams = _h;
		mU = _u;
	}

	
	void exec()
	{
		switch (mType)
		{
		case OT_ADD:
			break;
		case OT_SCALE:
			break;
		case OT_MIX:
			break;
		case OT_CUBIC:
			break;
		case OT_BILERP:
			break;
		case OT_BICUBIC:
			break;
		default:		//will just be identity
			break;
		}
	}
	


	OpType mType;
	HierarchicalBlendOperations mOp;		//operations
	std::vector<HierarchicalPose> mHParams;	//to be operated upon (controls)
	std::vector<float*> mU;					//u values, they are constantly updating so getting pointers will allow for not being passed
											//		during exec stage

	HierarchicalPose mOut;					//outpose
};

struct
{
	std::vector<BlendNode> getNodes() { return mNodes; }

	Hierarchy mBTH;	//blend tree hierarchy
	std::vector<BlendNode> mNodes;
};

struct ClipObject
{
	ClipObject(KeyframeObjectPool* kfpool, int firstKeyframe, int lastKeyframe)
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

	ClipObject(KeyframeObjectPool* kfpool, int firstKeyframe, int lastKeyframe, float fixedDuration)
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

	KeyframeObjectPool* mKeyframePool;//in the array, the clip will be the sequence of keyframes from FIRST to LAST. This is where decoupling and abstraction is used
								//		A CLIP DOESNT OWN KEYFRAMES, it just references a set of some that exist someplace else
	int mFirstKeyframeIndex;	//index of first keyframe in keyframe pool referenced by clip
	int mLastKeyframeIndex;		//index of last keyframe in keyframe pool referenced by clip

	bool mTransitionalForward;
	bool mTransitionalBackward;
};

struct ClipSkeletal
{
	ClipSkeletal(KeyframeSkeletalPool* kfpool, int firstKeyframe, int lastKeyframe)
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

	ClipSkeletal(KeyframeSkeletalPool* kfpool, int firstKeyframe, int lastKeyframe, float fixedDuration)
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

	KeyframeSkeletalPool* mKeyframePool;//in the array, the clip will be the sequence of keyframes from FIRST to LAST. This is where decoupling and abstraction is used
								//		A CLIP DOESNT OWN KEYFRAMES, it just references a set of some that exist someplace else
	int mFirstKeyframeIndex;	//index of first keyframe in keyframe pool referenced by clip
	int mLastKeyframeIndex;		//index of last keyframe in keyframe pool referenced by clip

	bool mTransitionalForward;
	bool mTransitionalBackward;
};

struct ClipObjectPool
{
	ClipObjectPool()
	{
		mClipCount = 0;
	}

	~ClipObjectPool()
	{
		//deallocate the pool
	}

	void addClip(ClipObject c)
	{
		++mClipCount;
		mClips.push_back(c);
	}

	int getClipByName(std::string name)		//search for the clip by name, return its index
	{
		for (ClipObject& itr : mClips)
		{
			if (itr.mName.compare(name) == 0)
			{
				return itr.mIndex;
				break;
			}
		}
	}

	std::vector<ClipObject> getClips() { return mClips; }
	


	int mClipCount;
	std::vector<ClipObject> mClips;
};

struct ClipSkeletalPool
{
	ClipSkeletalPool()
	{
		mClipCount = 0;
	}

	~ClipSkeletalPool()
	{
		//deallocate the pool
	}

	void addClip(ClipSkeletal c)
	{
		++mClipCount;
		mClips.push_back(c);
	}

	int getClipByName(std::string name)		//search for the clip by name, return its index
	{
		for (ClipSkeletal& itr : mClips)
		{
			if (itr.mName.compare(name) == 0)
			{
				return itr.mIndex;
				break;
			}
		}
	}

	std::vector<ClipSkeletal> getClips() { return mClips; }



	int mClipCount;
	std::vector<ClipSkeletal> mClips;
};

struct TransitionalsObject
{
public:

	TransitionalsObject()
	{
		mTargetClipIndex = 0;
		mClipPool = nullptr;
	}

	TransitionalsObject(ClipObjectPool* cP)
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

	void setClipPool(ClipObjectPool* cP) { mClipPool = cP; }
	void setTargetIndex(int tCI) { mTargetClipIndex = tCI; }


	int mTargetClipIndex;	//this will be either fed with a parameter of clipIndex + 1 or -1 depending for
							//		forward or backward
	ClipObjectPool* mClipPool;
};

struct TransitionalsSkeletal
{
public:

	TransitionalsSkeletal()
	{
		mTargetClipIndex = 0;
		mClipPool = nullptr;
	}

	TransitionalsSkeletal(ClipSkeletalPool* cP)
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

	void setClipPool(ClipSkeletalPool* cP) { mClipPool = cP; }
	void setTargetIndex(int tCI) { mTargetClipIndex = tCI; }


	int mTargetClipIndex;	//this will be either fed with a parameter of clipIndex + 1 or -1 depending for
							//		forward or backward
	ClipSkeletalPool* mClipPool;
};

class ClipControllerObject
{
public:
	
	ClipControllerObject(std::string name, ClipObjectPool* pool)
	{
		mName = name;
		mpClipPool = pool;
		mClipIndex = -1;	//if -1 then we dont have to clip to control yet so skip in the update
		mKeyframeIndex = 0;
		mSlowMoMultiplier = 1;
		mClipTime = 0.0;
		mKeyframeTime = 0.0;
		mTransitionalClipMode = false;
		mTransitionalManager = new TransitionalsObject(pool);
	}

	void update(float dt);

	void setClipPool(ClipObjectPool* pool) { mpClipPool = pool; }
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
	
	ClipObjectPool* mpClipPool;					//reference to the pool of clips to control
	TransitionalsObject* mTransitionalManager;
	bool mTransitionalClipMode;

};

class ClipControllerSkeletal
{
public:

	ClipControllerSkeletal(std::string name, ClipSkeletalPool* pool)
	{
		mName = name;
		mpClipPool = pool;
		mClipIndex = -1;	//if -1 then we dont have to clip to control yet so skip in the update
		mKeyframeIndex = 0;
		mSlowMoMultiplier = 1;
		mClipTime = 0.0;
		mKeyframeTime = 0.0;
		mTransitionalClipMode = false;
		mTransitionalManager = new TransitionalsSkeletal(pool);
	}

	void update(float dt);

	void setClipPool(ClipSkeletalPool* pool) { mpClipPool = pool; }
	void setClipToUseByIndex(int cpIndex) { mClipIndex = cpIndex; }
	void setStartToLastKeyframe() { mKeyframeIndex = mpClipPool->getClips().at(mClipIndex).mLastKeyframeIndex; }
	void setStartToFirstKeyframe() { mKeyframeIndex = mpClipPool->getClips().at(mClipIndex).mFirstKeyframeIndex; }
	void setTransitionalMode(bool mode) { mTransitionalClipMode = mode; }

	float& getSlowMoMultiplier() { return mSlowMoMultiplier; }
	int getCurrentKeyframeIndex() { return mKeyframeIndex; }
	int getClipIndexInPool() { return mClipIndex; }	//returns -1 when empty
	int& getPlaybackDirection() { return mPlaybackDirection; } //can be adjusted from ImGUI interface this way
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

	ClipSkeletalPool* mpClipPool;					//reference to the pool of clips to control
	TransitionalsSkeletal* mTransitionalManager;
	bool mTransitionalClipMode;

};
