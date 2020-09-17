#include "ClipController.h"


void ClipController::update(float dt)
{
	float timeStep = dt;
	timeStep *= mSlowMoMultiplier;
	timeStep *= mPlaybackDirection;	//this is why he suggusted -1 0 1

	if (mPlaybackDirection == 0) //7
	{
		//leave
		return;
	}

	//apply time step: one-time, SINGLE LINE OF CODE APPLIED TWICE to increment keyframe time and clip time by the step
	//		numerical integration for time itself
	mKeyframeTime += timeStep;
	mClipTime += timeStep;

	Clip tempClip = mpClipPool->getClips().at(mClipIndex);
	Keyframe tempKeyframe = mpClipPool->getClips().at(mClipIndex).mKeyframePool->getKeyframe(mKeyframeIndex);

//Resolve time: WHILE UNRESOLVED, continue to use playback behavior to determine the new keyframe time and clip time
//		update behavior should be 'looping' such that when the clip reaches either end, it starts playing again from the opposite end in the same dir
//		there are SEVEN CASES to check for resolution, atleast one of which definitively terminates algorithm:
//				1) Forward / same kf							4) Reverse / same kf				7) Pause (timestep is 0)
//				2) Forward / next kf (you have to RESOLVE)		5) Reverse / prior kf (RESOLVE)
//				3) Forward / end of Clip						6) Reverse / end of Clip

	
	if (mPlaybackDirection == 1)	//1, 2, 3
	{
		if (mKeyframeTime > tempKeyframe.mDuration)
		{
			mKeyframeIndex++;	//next frame
			mKeyframeTime -= tempKeyframe.mDuration;

			if (mKeyframeIndex > tempClip.mLastKeyframeIndex)	//if we are greater than the last keyframe going forward
			{
				tempClip.mTransitionalForward = true;

				if (mTransitionalClipMode)
				{
					mTransitionalManager->replaceClip(mClipIndex, mKeyframeIndex, mPlaybackDirection);
					//mTransitionalManager->replaceClip(mClipIndex, mKeyframeIndex);
				}
				else
				{
					mKeyframeIndex = tempClip.mFirstKeyframeIndex;	//this is the loop
					mClipTime = mKeyframeTime;
				}
			}
			else
			{
				tempClip.mTransitionalForward = false;
			}
		}
	}

	if (mPlaybackDirection == -1) //4, 5, 6
	{
		if (mKeyframeTime < 0)	//keyframe time not delta time, it can go this way
		{
			mKeyframeIndex--;

			if (mKeyframeIndex < tempClip.mFirstKeyframeIndex)
			{
				tempClip.mTransitionalBackward = true;

				if (mTransitionalClipMode)
				{
					//mTransitionalManager->replaceClip(mClipIndex, mKeyframeIndex);
					mTransitionalManager->replaceClip(mClipIndex, mKeyframeIndex, mPlaybackDirection);
				}
				else
				{
					mKeyframeIndex = tempClip.mLastKeyframeIndex;	//set to the end
					mClipTime = tempClip.mDuration + mKeyframeParameter;
				}
			}
			else
			{
				tempClip.mTransitionalBackward = false;
			}

			mKeyframeTime = tempKeyframe.mDuration + mKeyframeParameter;
		}
	}


//normalize time/ parameters: one-time, SINGLE LINE OF CODE APPLIED TWICE to calculate normalized keyframe time and clip time (between 0 and 1)
//			DO NOT CLAMP

	mClipParameter = mClipTime / tempClip.mDuration;
	mKeyframeParameter = mKeyframeTime / tempKeyframe.mDuration;
}