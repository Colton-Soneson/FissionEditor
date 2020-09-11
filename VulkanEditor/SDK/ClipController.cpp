#include "ClipController.h"

void ClipController::update(float dt)
{
	float timeStep = dt;
	timeStep *= mSlowMoMultiplier;

	//apply time step: one-time, SINGLE LINE OF CODE APPLIED TWICE to increment keyframe time and clip time by the step
	//		numerical integration for time itself
	mKeyframeTime += timeStep;
	mClipTime += timeStep;

	

//Resolve time: WHILE UNRESOLVED, continue to use playback behavior to determine the new keyframe time and clip time
//		update behavior should be 'looping' such that when the clip reaches either end, it starts playing again from the opposite end in the same dir
//		there are SEVEN CASES to check for resolution, atleast one of which definitively terminates algorithm:
//				1) Forward / same kf							4) Reverse / same kf				7) Pause (timestep is 0)
//				2) Forward / next kf (you have to RESOLVE)		5) Reverse / prior kf (RESOLVE)
//				3) Forward / end of Clip						6) Reverse / end of Clip

//normalize time/ parameters: one-time, SINGLE LINE OF CODE APPLIED TWICE to calculate normalized keyframe time and clip time (between 0 and 1)
//			DO NOT CLAMP
}