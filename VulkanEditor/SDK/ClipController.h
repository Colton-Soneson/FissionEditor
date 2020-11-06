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
	BlendNode(OpType _op, std::vector<HierarchicalPose*> _h, std::vector<float*> _u)
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
			mOut = mOp.merge(mHParams.at(0), mHParams.at(1));
			break;
		case OT_SCALE:
			mOut = mOp.scale(mHParams.at(0), *(mU.at(0)));
			break;
		case OT_MIX:
			mOut = mOp.mix(mHParams.at(0), mHParams.at(1), *(mU.at(0)));
			break;
		case OT_CUBIC:
			mOut = mOp.cubic(mHParams.at(0), mHParams.at(1), mHParams.at(2), mHParams.at(3), *(mU.at(0)));
			break;
		case OT_BILERP:
			mOut = mOp.bilerp(mHParams.at(0), mHParams.at(1), mHParams.at(2), mHParams.at(3), *(mU.at(0)), *(mU.at(1)), *(mU.at(2)));
			break;
		case OT_BICUBIC:
			mOut = mOp.bicubic(mHParams.at(0), mHParams.at(1), mHParams.at(2), mHParams.at(3),
				mHParams.at(4), mHParams.at(5), mHParams.at(6), mHParams.at(7),
				mHParams.at(8), mHParams.at(9), mHParams.at(10), mHParams.at(11),
				mHParams.at(12), mHParams.at(13), mHParams.at(14), mHParams.at(15),
				*(mU.at(0)), *(mU.at(1)), *(mU.at(2)), *(mU.at(3)), *(mU.at(4)));
			break;
		default:		//will just be identity
			mOut = mOp.identity();
			break;
		}
	}
	
	OpType mType;
	HierarchicalBlendOperations mOp;		//operations
	std::vector<HierarchicalPose*> mHParams;	//to be operated upon (controls)
	std::vector<float*> mU;					//u values, they are constantly updating so getting pointers will allow for not being passed
											//		during exec stage

	HierarchicalPose* mOut;					//outpose
};

struct BlendTree
{
	void addToTree(BlendNode _n)
	{
		mNodes.push_back(_n);
	}
	
	void updateTree()	//just run the exec functions on all nodes
	{
		for (int i = 0; i < mBTH.getNumberOfNodes(); ++i)
		{
			mNodes.at(i).exec();	//this is still acting on individual hierarchyPose nodes, not the full set
		}
	}

	std::vector<BlendNode> getNodes() { return mNodes; }

	Hierarchy mBTH;	//blend tree hierarchy
	std::vector<BlendNode> mNodes;
};

//input based node
struct InputNode
{
	InputNode(LocomotionControlType _LeftLCT, LocomotionControlType _RightLCT, float _lSpeed, float _rSpeed)
	{
		mLeftLCT = _LeftLCT;
		mRightLCT = _RightLCT;
		mLeftDirectionals = glm::vec2(0.0);
		mRightDirectionals = glm::vec2(0.0);

		mLeftP = glm::vec4(0.0);
		mLeftV = glm::vec4(0.0);
		mLeftA = glm::vec4(0.0);

		mRightP = glm::vec4(0.0);
		mRightV = glm::vec4(0.0);
		mRightA = glm::vec4(0.0);

		mLeftSpeed = _lSpeed;
		mRightSpeed = _rSpeed;
	}

	InputNode()
	{
		mLeftLCT = LCT_DV;		//default the direct value
		mRightLCT = LCT_DV;		//default the direct value
		mLeftDirectionals = glm::vec2(0.0);
		mRightDirectionals = glm::vec2(0.0);

		mLeftP = glm::vec4(0.0);
		mLeftV = glm::vec4(0.0);
		mLeftA = glm::vec4(0.0);

		mRightP = glm::vec4(0.0);
		mRightV = glm::vec4(0.0);
		mRightA = glm::vec4(0.0);

		mLeftSpeed = 1.0;
		mRightSpeed = glm::pi<float>();
	}

	void LCTupdate(float dt, float u0, float u1)
	{
		switch (mLeftLCT)	//positional movement
		{
		case LCT_DV:
			mLeftP = glm::vec4(mLeftDirectionals.x * mLeftSpeed, mLeftDirectionals.y * mLeftSpeed, 0.0, 0.0);
			break;
		case LCT_CV:
			mLeftV = glm::vec4(mLeftDirectionals.x * mLeftSpeed, mLeftDirectionals.y * mLeftSpeed, 0.0, 0.0);
			mLeftP = fIntegratedEuler(mLeftP, mLeftV, dt);
			break;
		case LCT_CA:
			mLeftA = glm::vec4(mLeftDirectionals.x * mLeftSpeed, mLeftDirectionals.y * mLeftSpeed, 0.0, 0.0);
			mLeftV = fIntegratedEuler(mLeftV, mLeftA, dt);
			mLeftP = fIntegratedEuler(mLeftP, mLeftV, dt);
			break;
		case LCT_FV:
			mLeftP = fIntegrateKinematic(glm::vec4(mLeftDirectionals.x * mLeftSpeed, mLeftDirectionals.y * mLeftSpeed, 0.0, 0.0), mLeftV, mLeftA, u0);
			break;
		case LCT_FA:
			mLeftP = fIntegrateKinematic(glm::vec4(mLeftDirectionals.x * mLeftSpeed, mLeftDirectionals.y * mLeftSpeed, 0.0, 0.0), mLeftV, mLeftA, u0);
			break;
		}

		switch (mRightLCT)	//rotational movement
		{
		case LCT_DV:
			mRightP = glm::vec4(mRightDirectionals.x * mRightSpeed, mRightDirectionals.y * mRightSpeed, 0.0, 0.0);
			break;
		case LCT_CV:
			mRightV = glm::vec4(mRightDirectionals.x * mRightSpeed, mRightDirectionals.y * mRightSpeed, 0.0, 0.0);
			mRightP = fIntegratedEuler(mRightP, mRightV, dt);
			break;
		case LCT_CA:
			mRightA = glm::vec4(mRightDirectionals.x * mRightSpeed, mRightDirectionals.y * mRightSpeed, 0.0, 0.0);
			mRightV = fIntegratedEuler(mRightV, mRightA, dt);
			mRightP = fIntegratedEuler(mRightP, mRightV, dt);
			break;
		case LCT_FV:
			mRightP = fIntegrateKinematic(glm::vec4(mRightDirectionals.x * mRightSpeed, mRightDirectionals.y * mRightSpeed, 0.0, 0.0), mRightV, mRightA, u1);
			break;
		case LCT_FA:
			mRightP = fIntegrateKinematic(glm::vec4(mRightDirectionals.x * mRightSpeed, mRightDirectionals.y * mRightSpeed, 0.0, 0.0), mRightV, mRightA, u1);
			break;
		}
	}

	void update(std::vector<int> _iList, float dt, float u0, float u1)
	{
		//setting the inputs based on what was given from GLFW
		for (int i = 0; i < _iList.size() - 1; ++i)
		{
			if (_iList.at(i) == OWI_I) { mRightDirectionals = glm::vec2(mRightDirectionals.x, mRightDirectionals.y + 1); }
			else if (_iList.at(i) == OWI_K) { mRightDirectionals = glm::vec2(mRightDirectionals.x, mRightDirectionals.y - 1); }
			else if (_iList.at(i) == OWI_J) { mRightDirectionals = glm::vec2(mRightDirectionals.x - 1, mRightDirectionals.y); }
			else if (_iList.at(i) == OWI_L) { mRightDirectionals = glm::vec2(mRightDirectionals.x + 1, mRightDirectionals.y); }
			else if (_iList.at(i) == OWI_W) { mLeftDirectionals = glm::vec2(mLeftDirectionals.x, mLeftDirectionals.y + 1); }
			else if (_iList.at(i) == OWI_S) { mLeftDirectionals = glm::vec2(mLeftDirectionals.x, mLeftDirectionals.y - 1); }
			else if (_iList.at(i) == OWI_A) { mLeftDirectionals = glm::vec2(mLeftDirectionals.x + 1, mLeftDirectionals.y); }
			else if (_iList.at(i) == OWI_D) { mLeftDirectionals = glm::vec2(mLeftDirectionals.x - 1, mLeftDirectionals.y); }
		}

		LCTupdate(dt, u0, u1);

		//this reset is called at the very end
		mLeftDirectionals = glm::vec2(0.0);
		mRightDirectionals = glm::vec2(0.0);
	}

	glm::vec4 fIntegratedEuler(glm::vec4 x, glm::vec4 dx_dt, float dt)	//dx_dt aka velocity
	{
		return x + (dx_dt * dt);
	}

	glm::vec4 fIntegrateKinematic(glm::vec4 x, glm::vec4 dx_dt, glm::vec4 d2x_dt2, float dt)	//d2x_dt2 aka acceleration
	{
		return x + (dx_dt * dt) + ((d2x_dt2 * dt * dt) / 2.0f);
	}

	glm::vec4 fIntegrateInterpolated(glm::vec4 x, glm::vec4 xc, float u)
	{
		return x + (xc - x) * u;
	}


private:
	glm::vec2 mLeftDirectionals;
	glm::vec2 mRightDirectionals;
	std::vector<bool*> mConditionalList;
	LocomotionControlType mLeftLCT;
	LocomotionControlType mRightLCT;

	glm::vec4 mLeftP;
	glm::vec4 mLeftV;
	glm::vec4 mLeftA;
	
	glm::vec4 mRightP;
	glm::vec4 mRightV;
	glm::vec4 mRightA;

	float mLeftSpeed;	//movement speed only
	float mRightSpeed;	//rotation speed only
};

struct InputTree
{
	void addToTree(InputNode _n)
	{
		mNodes.push_back(_n);
	}

	void updateTree(std::vector<int> _iList, float dt, float u0, float u1)	//just run the exec functions on all nodes
	{
		for (int i = 0; i < mITH.getNumberOfNodes(); ++i)
		{
			mNodes.at(i).update(_iList, dt, u0, u1);	//this is still acting on individual hierarchyPose nodes, not the full set
		}
	}

	std::vector<InputNode> getNodes() { return mNodes; }

	Hierarchy mITH;	//blend tree hierarchy
	std::vector<InputNode> mNodes;
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

	BlendTree mBT;	//blend tree for clip
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
	void setUValues(std::vector<float*> _u) { mU = _u; }

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

	std::vector<float*> mU;

};
