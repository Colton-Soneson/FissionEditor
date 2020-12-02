#pragma once
#include "ClipController.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

struct HTR_Position
{
	std::string name;
	float Tx, Ty, Tz, Rx, Ry, Rz, BoneLength;
};

struct HumanoidBasic
{
	//	Humanoid Basic Node Hierarchy (depth first)
	/*
	//			0
	//			|
	//			1
	//			|
	//			2
	//		  / | \	
	//		 3  4  5
	//		/  / \  \
	//	   6  8  11  14
	//	  /  /     \   \
	//   7  9       12  15
	//	   /         \
	//	  10          13
	*/

	//	Humanoid Basic Spatial Node Hierarchy (depth first)
	/*
	//			0				Face (Root)
	//			|	
	//			1				Neck
	//			|
	//			2				BaseNeck
	//		  / | \	
	//		 3  |  5			Shoulder0, Shoulder1
	//		/   |   \
	//	   6    |    14			Elbow0, Elbow1
	//	  /		|	  \
	//	 7      |	   15		Hand0, Hand1
	//			|
	//		    4				Hipbone
	//		  /   \
	//		 8     11			Hip0, Hip1
	//	    /		\
	//	   9		 12			Knee0, Knee1
	//	   |		 |
	//	   10		 13			Foot0, Foot1
	*/


	//	Humanoid Basic Node Hierarchy (#Row, #LeafInBranch)
	/*
	//			  (0,0)				Row 0
	//				|
	//			  (1,0)				Row 1
	//				|
	//			  (2,0)				Row 2
	//	       /    |    \
	//	   (3,0)  (3,1)  (3,2)		Row 3
	//		/	 /     \	 \
	//	 (4,0) (4,0)  (4,1)  (4,0)	Row 4
	//	   |    |		 |	   |
	//	 (5,0) (5,0)  (5,0)  (5,0)	Row 5
	//		    |        |
	//		   (6,0)  (6,0)			Row 6
	*/

	HumanoidBasic()
	{
		mpHierarchy = new Hierarchy();
		mpHierarchicalPosePool = new HierarchicalPosePool(mpHierarchy);
		mNumberOfPoses = 0;
		mActiveHierarchicalState = 0;
	}

	void createBasePose()
	{
		//create a hierarchy with enough nodes to represent skeleton
		//initialize a hierarchical pose pool
		//initialize first pose in the pool to be base pose

		Joint temp;

		mpHierarchy->addNode(Node("Face (ROOT)", 0, 0));
		temp.setPosition(glm::vec4(0.0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Neck", 1, 0));
		temp.setPosition(glm::vec4(0, 0, -1, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("BaseNeck", 2, 1));
		temp.setPosition(glm::vec4(0, 0, -2, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Shoulder0", 3, 2));
		temp.setPosition(glm::vec4(-3, 0, -2, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Shoulder1", 5, 2));
		temp.setPosition(glm::vec4(+3, 0, -2, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Elbow0", 6, 3));
		temp.setPosition(glm::vec4(-3, 0, -6, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Elbow1", 14, 5));
		temp.setPosition(glm::vec4(+3, 0, -6, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Hand0", 7, 6));
		temp.setPosition(glm::vec4(-3, 0, -10, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Hand1", 15, 14));
		temp.setPosition(glm::vec4(+3, 0, -10, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Hipbone", 4, 2));
		temp.setPosition(glm::vec4(0, 0, -12, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Hip0", 8, 4));
		temp.setPosition(glm::vec4(-2.5, 0, -11.75, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Hip1", 11, 4));
		temp.setPosition(glm::vec4(+2.5, 0, -11.75, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Knee0", 9, 8));
		temp.setPosition(glm::vec4(-2.5, 0, -16, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Knee1", 12, 11));
		temp.setPosition(glm::vec4(+2.5, 0, -16, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Foot0", 10, 9));
		temp.setPosition(glm::vec4(-2.5, 0, -20, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Foot1", 13, 12));
		temp.setPosition(glm::vec4(+2.5, 0, -20, 0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		/*
		mpHierarchy->addNode(Node("Face (ROOT)", 0, 0));
		temp.setPosition(glm::vec4(0.0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Neck", 1, 0));
		temp.setPosition(glm::vec4(0,0,-1,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);
		
		mpHierarchy->addNode(Node("BaseNeck", 2, 1));
		temp.setPosition(glm::vec4(0,0,-2,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Shoulder0", 3, 2));
		temp.setPosition(glm::vec4(-5,0,+0.75,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Shoulder1", 5, 2));
		temp.setPosition(glm::vec4(+5,0,+0.75,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Elbow0", 6, 3));
		temp.setPosition(glm::vec4(-1,0,-8,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Elbow1", 14, 5));
		temp.setPosition(glm::vec4(+1,0,-8,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Hand0", 7, 6));
		temp.setPosition(glm::vec4(-1,0,-8,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Hand1", 15, 14));
		temp.setPosition(glm::vec4(+1,0,-8,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Hipbone", 4, 2));
		temp.setPosition(glm::vec4(0,0,-14,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Hip0", 8, 4));
		temp.setPosition(glm::vec4(-3.5,0,+0.5,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Hip1", 11, 4));
		temp.setPosition(glm::vec4(+3.5,0,+0.5,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Knee0", 9, 8));
		temp.setPosition(glm::vec4(0,0,-9,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Knee1", 12, 11));
		temp.setPosition(glm::vec4(0,0,-9,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Foot0", 10, 9));
		temp.setPosition(glm::vec4(0,0,-6,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);

		mpHierarchy->addNode(Node("Foot1", 13, 12));
		temp.setPosition(glm::vec4(0,0,-6,0));
		mpHierarchicalPosePool->addToSpatialPosePool(temp);
		*/


		mpHierarchicalPosePool->addToHierarchicalPoses(mNumberOfPoses);
		mNumberOfPoses++;
		
	}

	//use addKeyPose to create 3 more poses that work from base pose
	void addKeyPose()
	{
		//taking in the base pose, create modifications
		//HierarchicalPose tempH = mpHierarchicalPosePool->getHierarchicalPoseGroups().mHPGS.at(0).mHPG;	
		//Joint tempJ;
		//tempJ.copy(&tempJ, tempH.mHierarchicalJoints.at(5));
		//tempJ.setPosition(glm::vec4(0, +5, 0, 0));
		//tempH.mHierarchicalJoints.at(5) = &tempJ;
		//mpHierarchicalPosePool->addToHierarchicalPoses(tempH);

	}

	std::string getNodeNameList()
	{
		std::string temp;

		for (int i = 0; i < mpHierarchy->getNumberOfNodes(); ++i)
		{
			temp += mpHierarchy->getNodes().at(i).mName;
			temp += ", ";
		}

		return temp;
	}
	
	void createHierarchicalState()
	{
		HierarchicalState* newState = new HierarchicalState(mpHierarchy);
	}

	void calculateGlobalSpacePose()
	{
		for (int i = 0; i < mpHierarchicalStates.size() - 1; ++i)
		{
			for (int j = 0; j < mpHierarchy->getNumberOfNodes(); ++j)
			{
				//interpolation (between deltas/keyposes)
				//mpHierarchicalStates.at(i)->mAnimatedPose.mHierarchicalJoints.at(j)->lerp();

				//concatenation (do current delta concat with base pose)
				//mpHierarchicalStates.at(i)->mAnimatedPose.mHierarchicalJoints.at(j)->concatenate();

				//conversion (convert base to local)
				//mpHierarchicalStates.at(i)->mAnimatedPose.mHierarchicalJoints.at(j)->convertToMat();
			}
			//FK equation (goes from local to object space)
			mFK.fkAlgorithm(mpHierarchicalStates.at(i));
		}
	}

	int mActiveHierarchicalState;
	int mNumberOfPoses;
	Hierarchy* mpHierarchy;
	HierarchicalPosePool* mpHierarchicalPosePool;
	std::vector<HierarchicalState*> mpHierarchicalStates;
	ForwardKinematics mFK;
};

struct HTRSkeleton
{
	HTRSkeleton(std::string filePath)
	{
		mFilePath = filePath;
		mpHierarchy = new Hierarchy();
		mpHierarchicalPosePool = new HierarchicalPosePool(mpHierarchy);
		mNumberOfPoses = 0;
		mActiveHierarchicalState = 0;
	}

	void loadHTRFile()
	{
		std::ifstream htr(mFilePath.c_str());
		std::string line;
		int lineCounterForChildren = 0;
		std::string cutWord;
		int currentAnimationFrame = 1;	//IN THE FILE IT STARTS AT 1
		bool basePoseComplete = false;
		//std::vector<HTR_Position> framePose;

		
		if (htr.is_open())
		{
			
			while (std::getline(htr, line))
			{
				std::istringstream iss(line);
				std::string word;
				bool skip = false;

				//determine stage of file, and beginning of stage executions
				if (line[0] == '#')
				{
					skip = true;
				}
				else if (mStages.at(0) == line)
				{
					mStageFlags.at(0) = true;
					mStageFlags.at(1) = false;
					mStageFlags.at(2) = false;
					mStageFlags.at(3) = false;
					skip = true;
					std::cout << "\n HTR LOAD 0% COMPLETE";
				}
				else if (mStages.at(1) == line)
				{
					mStageFlags.at(0) = false;
					mStageFlags.at(1) = true;
					mStageFlags.at(2) = false;
					mStageFlags.at(3) = false;
					skip = true;
					std::cout << "\n HTR LOAD 25% COMPLETE";
				}
				else if (mStages.at(2) == line)
				{
					mStageFlags.at(0) = false;
					mStageFlags.at(1) = false;
					mStageFlags.at(2) = true;
					mStageFlags.at(3) = false;
					skip = true;
					std::cout << "\n HTR LOAD 50% COMPLETE";
				}
				else if (mStages.at(3) == line)
				{
					OrganizeNodesDepthFirst();

					mStageFlags.at(0) = false;
					mStageFlags.at(1) = false;
					mStageFlags.at(2) = false;
					mStageFlags.at(3) = true;

					skip = true;
					std::cout << "\n HTR LOAD 75% COMPLETE";
				}

				//if we checked the stage line this true and we dont compute the "[ EXAMPLE ]" word
				if (skip == false)
				{
					//actions for each stage
					if (mStageFlags.at(0))
					{
						do
						{
							iss >> word;	//set word of line

							if (word[0] == '#')	//if we encounter the comment symbol ditch the word parser and therefore the line
							{
								break;
							}
							else if (word == "FileType")
							{
								iss >> word;	//get the second word
								mFileType = word;
								break;
							}
							else if (word == "DataType")
							{
								iss >> word;	//get the second word
								mDataType = word;
								break;
							}
							else if (word == "FileVersion")
							{
								iss >> word;	//get the second word
								mFileVersion = std::stoi(word);
								break;
							}
							else if (word == "NumSegments")
							{
								iss >> word;	//get the second word
								mNumSegments = std::stoi(word);
								break;
							}
							else if (word == "NumFrames")
							{
								iss >> word;	//get the second word
								mNumFrames = std::stoi(word);
								mFramePoses.resize(mNumFrames);
								break;
							}
							else if (word == "DataFrameRate")
							{
								iss >> word;	//get the second word
								mDataFrameRate = std::stoi(word);
								break;
							}
							else if (word == "EulerRotationOrder")
							{
								iss >> word;	//get the second word
								mEulerRotationOrder = stringToEulerOrderConversion(word);							//GET BACK TO THIS TO ASSIGN PROPER CHANNEL, NOT JUST STRING
								break;
							}
							else if (word == "CalibrationUnits")
							{
								iss >> word;	//get the second word
								mCalibrationUnits = word;
								break;
							}
							else if (word == "RotationUnits")
							{
								iss >> word;	//get the second word
								mRotationUnits = word;
								break;
							}
							else if (word == "GlobalAxisofGravity")
							{
								iss >> word;	//get the second word
								mGlobalAxisOfGravity = word;
								break;
							}
							else if (word == "BoneLengthAxis")
							{
								iss >> word;	//get the second word
								mBoneLengthAxis = word;
								break;
							}
							else if (word == "ScaleFactor")
							{
								iss >> word;	//get the second word
								mScaleFactor = std::stof(word);
								break;
							}

						} while (iss);

					}
					else if (mStageFlags.at(1))
					{
						
						iss >> word;	//set word of line

						if (word[0] == '#')	//if we encounter the comment symbol ditch the word parser and therefore the line
						{
							break;
						}

						mChildren.push_back(word);

						iss >> word;

						mParent.push_back(word);

					

					}
					else if (mStageFlags.at(2))
					{
						do
						{
							iss >> word;	//set word of line

							bool doubleBreak  = false;

							if (word[0] == '#') { break; }

							
							//BASE POSE SET
							if (lineCounterForChildren < mChildren.size() - 1 && basePoseComplete == false)
							{
								HTR_Position htrp;

								htrp.name = word;

								iss >> word;
								htrp.Tx = std::stof(word);

								iss >> word;
								htrp.Ty = std::stof(word);

								iss >> word;
								htrp.Tz = std::stof(word);

								iss >> word;
								htrp.Rx = std::stof(word);

								iss >> word;
								htrp.Ry = std::stof(word);

								iss >> word;
								htrp.Rz = std::stof(word);

								iss >> word;
								htrp.BoneLength = std::stof(word);

								mBasePose.push_back(htrp);

								lineCounterForChildren++;
							}

							
							//done with base pose
							if (lineCounterForChildren == mChildren.size() - 1)
							{
								basePoseComplete = true;
								lineCounterForChildren = -1;	//this is so we can immediately do a plus 1 after first joint is called for anim data
							}
							

							if(basePoseComplete == true)
							{
								//at this point we still have the children node list in order of how the file read it
								//set current "[ JOINT NAME ]" option if we are on that line
								if (word[0] == '[')
								{
									cutWord = word;
									cutWord.erase(cutWord.begin() + 1, cutWord.end() - 1);
									lineCounterForChildren++;								//this allows us to go through all kiddos per animation frame
									currentAnimationFrame = 0;								//since we check for [ word first that means we dont have to track what animframe limit
								}
								else if(word[0] == '#')
								{
									//handle the second line of comments
									doubleBreak = true;
								}
								else
								{

									iss >> word;	//this would now be at the frame number in front of everything
									iss >> word;	//now we are at content of that frame

									mFramePoses.at(currentAnimationFrame).at(lineCounterForChildren).Tx = std::stof(word);

									iss >> word;
									mFramePoses.at(currentAnimationFrame).at(lineCounterForChildren).Ty = std::stof(word);

									iss >> word;
									mFramePoses.at(currentAnimationFrame).at(lineCounterForChildren).Tz = std::stof(word);

									iss >> word;
									mFramePoses.at(currentAnimationFrame).at(lineCounterForChildren).Rx = std::stof(word);

									iss >> word;
									mFramePoses.at(currentAnimationFrame).at(lineCounterForChildren).Ry = std::stof(word);

									iss >> word;
									mFramePoses.at(currentAnimationFrame).at(lineCounterForChildren).Rz = std::stof(word);

									iss >> word;
									mFramePoses.at(currentAnimationFrame).at(lineCounterForChildren).BoneLength = std::stof(word);
									
									currentAnimationFrame++;
								}
								
							}

							
							if (doubleBreak == true)
							{
								break;
							}



						} while (iss);
					}
				}
			}
			
		}

	}

	JointEulerOrder stringToEulerOrderConversion(std::string s)
	{
		if (s.compare("xyz"))
		{
			return (JointEulerOrder)jointChannel_orient_xyz;
		}
		else if (s.compare("yzx"))
		{
			return (JointEulerOrder)jointEulerOrder_yzx;
		}
		else if (s.compare("zxy"))
		{
			return (JointEulerOrder)jointEulerOrder_zxy;
		}
		else if (s.compare("yxz"))
		{
			return (JointEulerOrder)jointEulerOrder_yxz;
		}
		else if (s.compare("xzy"))
		{
			return (JointEulerOrder)jointEulerOrder_xzy;
		}
		else if (s.compare("zyx"))
		{
			return (JointEulerOrder)jointEulerOrder_zyx;
		}
		else
		{
			return (JointEulerOrder)jointEulerOrder_xyz;
		}
		
	}

	void OrganizeNodesDepthFirst()
	{
		//go through whole list of child v parent nodes to find what child has "GLOBAL" as parent index
		int indexOfRootNodeInChildren;
		for (int i = 0; i < mParent.size() - 1; ++i)
		{
			if (mParent.at(i).compare("GLOBAL"))
			{
				indexOfRootNodeInChildren = i;
			}
		}

		//set the node attached to GLOBAL equal to zero as well in the children list

		std::vector<std::pair<std::string, int>> childrenWithIndex;
		std::vector<std::pair<std::string, int>> parentWithIndex;

		for (int i = 0; i < mChildren.size() - 1; ++i)
		{
			//childrenWithIndex.emplace(std::pair<std::string, int>(mChildren.at(i), i));	//fill children list
			//parentWithIndex.emplace(std::pair<std::string, int>(mParent.at(i), i));	//fill children list

			childrenWithIndex.push_back(std::pair<std::string, int>(mChildren.at(i), i));
			parentWithIndex.push_back(std::pair<std::string, int>(mParent.at(i), i));
		}

		//fill hierarchy of parents
		//for (int i = 0; i < mParent.size() - 1; ++i) { hierarchy.emplace(std::pair<int, std::vector<int>>(i, std::vector<int>(0.0))); }

		for (int i = 0; i < mChildren.size() - 1; ++i)
		{
			for (int j = 0; j < mParent.size() - 1; ++j)
			{
				if (mParent.at(j) == mChildren.at(i))
				{
					//parentWithIndex.at(mParent.at(j)) = childrenWithIndex.find(mChildren.at(i))
					
				}
			}
		}

		/*std::string previousJointName;
		for (int itrPos = indexOfRootNodeInChildren; itrPos < mChildren.size() - 1; ++itrPos)
		{
				
		}

		for (int itrNeg = indexOfRootNodeInChildren; itrNeg >= 0; --itrNeg)
		{

		}*/

		//std::vector<std::map<int, std::string>> childrenNodesMap;
		//std::vector<std::map<int, std::string>> parentNodesMap;


		//std::vector<int> newParentPositions;

	//	for (int i = 0; i < mChildren.size() - 1; ++i)
	//	{

	//	}




	//}

	//void DFSTraversal(int v, bool visited[])
	//{
	//	visited[v] = true;	//current node visited
	//	std::vector<int>::iterator i;
	//	for(i = )

	}

	//void createBasePose()
	//{

	//}

	//stages into file
	//	specific to HTR file
	std::vector<std::string> mStages = {"[Header]", "[SegmentNames&Hierarchy]", "[BasePosition]", "[EndOfFile]"};
	std::vector<bool> mStageFlags = {false, false, false, false};

	//file details
	std::string mFilePath;
	std::string mFileType;
	std::string mDataType;
	int mFileVersion;
	
	//animation specifics
	int mNumSegments;
	int mNumFrames;
	int mDataFrameRate;
	JointEulerOrder mEulerRotationOrder;
	std::string mCalibrationUnits;
	std::string mRotationUnits;
	std::string mGlobalAxisOfGravity;
	std::string mBoneLengthAxis;
	float mScaleFactor;

	//segment names and hierarchy
	//	(with htr, root node's parent is "GLOBAL")
	std::vector<std::string> mChildren;
	std::vector<std::string> mParent;
	std::vector<HTR_Position> mBasePose;
	std::vector<std::vector<HTR_Position>> mFramePoses;



	//This editor
	int mActiveHierarchicalState;
	int mNumberOfPoses;
	Hierarchy* mpHierarchy;
	HierarchicalPosePool* mpHierarchicalPosePool;
	std::vector<HierarchicalState*> mpHierarchicalStates;
	ForwardKinematics mFK;
};

//incase later on we add more skeletons
struct SkeletonContainer
{
	std::vector<HumanoidBasic> mHumanoidBasics;
	std::vector<HTRSkeleton> mHTRSkeletons;
};

class SkeletonManager
{
public:
	SkeletonManager() {};
	~SkeletonManager() {};

	void update();
	void createHumanoidBasic();
	void createHTRSkeleton();

	SkeletonContainer getSkeletonContainer() { return mSkeletonContainer; }

private:
	SkeletonContainer mSkeletonContainer;

};

/*
	Although this is "look", it doesn't have to be 
	just a "neck" bone to recieve this, Kinematics should resolve other joints
*/
struct ProceduralLook
{
	/*
		this should have the input of the hierarchical joint from the active skeleton
	*/
	ProceduralLook(Joint* target, glm::vec3 up)
	{
		mTarget = target;
		mUpDir = glm::normalize(up);
		mTargetJointPos = target->mPos;
	}

	void LookAtMatrixConv(glm::vec3 lookPoint)
	{
		//smoothing isnt done in here, so dont worry about it

		glm::vec3 pointToJointDir = glm::normalize(lookPoint - mTargetJointPos);
		glm::vec3 rightDir = glm::normalize(glm::cross(pointToJointDir, mUpDir));
		glm::vec3 perpUpDir = glm::cross(rightDir, pointToJointDir);	//make the up vector perp to the right
	
		mTarget->mTransform[0][0] = rightDir.x;
		mTarget->mTransform[1][0] = rightDir.y;
		mTarget->mTransform[2][0] = rightDir.z;
		mTarget->mTransform[0][1] = perpUpDir.x;
		mTarget->mTransform[1][1] = perpUpDir.y;
		mTarget->mTransform[2][1] = perpUpDir.z;
		mTarget->mTransform[0][2] = -pointToJointDir.x;
		mTarget->mTransform[1][2] = -pointToJointDir.y;
		mTarget->mTransform[2][2] = -pointToJointDir.z;
		mTarget->mTransform[3][0] = -glm::dot(rightDir, mTargetJointPos);
		mTarget->mTransform[3][1] = -glm::dot(perpUpDir, mTargetJointPos);
		mTarget->mTransform[3][2] = glm::dot(pointToJointDir, mTargetJointPos);
	};

	Joint* mTarget;
	glm::vec3 mUpDir;
	glm::vec3 mTargetJointPos;
};

/*
	use the ik process from the slide / FABRIK approach
*/
struct ProceduralGrab
{
	ProceduralGrab(Joint* baseEffector, Joint* endEffector, glm::vec4* platformPos, HierarchicalPosePool* skeleton, int baseEffectorNode, int endEffectorNode)
	{
		mpBaseEffector = baseEffector;
		mpEndEffector = endEffector;
		mBEN = baseEffectorNode;	//use to determine place in hierarchy
		mEEN = endEffectorNode;
		mpSkeleton = skeleton;

		mChainLength = 0;
		mEffectorDistance = 0;

		if (platformPos != nullptr)
		{
			mpPlatPos = platformPos;
		}

		fillJointList();
	}

	/*
		create easily accessable list of joint pointers that you dont have to 
		cycle through getting the data for index positions
	*/
	void fillJointList()
	{
		//assume jointList is sorted from baseEffector to endEffector
		//use mBEN index and mEEN index to get and integer list of indices to then translate to joint pointers
		int nextJointIndex = mBEN;
		while (nextJointIndex != mEEN)
		{
			for (int i = 0; i < mpSkeleton->getHierarchy()->getNumberOfNodes(); ++i)
			{
				if (mpSkeleton->getHierarchy()->getNodes().at(i).mIndex == nextJointIndex)
				{
					mJointList.push_back(mpSkeleton->getJoint(nextJointIndex, 0));

					//assign the nextJoint to be parent of joint
					nextJointIndex = mpSkeleton->getHierarchy()->getNodes().at(i).mParentIndex;
				}
			}
		}

		//calculate chain length after jointlist is filled
		for (int i = 0; i < mJointList.size() - 2; ++i)			//get actual distance between the joints, like physical distance
		{
			float dist = glm::length(mJointList.at(i + 1)->mPos - mJointList.at(i)->mPos);
			mDistances.push_back(dist);
			mChainLength += dist;
		}

		//calculate effector distance
		mEffectorDistance = glm::distance(mJointList.front()->mPos, mJointList.back()->mPos);
	}

	/*
		grabs with any amount of joints available, but in this case it does disregaurd the joint constraints
		iterations is how many times we want to repeat for better accuracy
	*/
	void SolveInverseKinematicsIter(glm::vec4 targetPos, int iterations)	
	{
		if (mJointList.empty() || mEffectorDistance >= mChainLength)
		{
			return;
		}

		int iter = 0;
		if (iterations <= 0)
		{
			iter = 1;
		}

		glm::vec4 firstBasePos = mJointList.at(0)->mPos;	//record original root location

		for (int i = 0; i < iter; ++i)
		{
			mJointList.back()->mPos = targetPos;	//move the endEffector to the target

			//go through the chain from end to beginning
			for (int i = mJointList.size(); i > 1; --i)		//has to be this weird way for parents
			{
				Joint* currJoint = mJointList.at(i);
				Joint* parentJoint = mJointList.at(i - 1);

				glm::vec4 dir = glm::normalize(parentJoint->mPos - currJoint->mPos);	//set the direction vector to place the parent joint inline

				parentJoint->mPos = currJoint->mPos - (mDistances.at(i - 1) * dir);	//set position of parent joint in the direction of the joint movement by the distance between
			}

			mJointList.at(0)->mPos = firstBasePos;		//move the beginning joint back to its root pos

			for (int i = 0; i < mJointList.size() - 1; ++i)
			{
				Joint* currJoint = mJointList.at(i);
				Joint* childJoint = mJointList.at(i + 1);

				glm::vec4 dir = glm::normalize(childJoint->mPos - currJoint->mPos);
				childJoint->mPos = currJoint->mPos + (mDistances.at(i) * dir);
			}
		}
	}

	void SolveInverseKinematicsMOE(glm::vec4 targetPos, float marginOfError)
	{
		if (mJointList.empty() || mEffectorDistance >= mChainLength)
		{
			return;
		}

		if (marginOfError <= 0)
		{
			std::cout << "Margin of error at or below 0\n";
			return;
		}

		glm::vec4 firstBasePos = mJointList.at(0)->mPos;	//record original root location

		while (!passMarginOfError(mJointList.back()->mPos, targetPos, marginOfError))
		{
			mJointList.back()->mPos = targetPos;	//move the endEffector to the target

			//go through the chain from end to beginning
			for (int i = mJointList.size(); i > 1; --i)		//has to be this weird way for parents
			{
				Joint* currJoint = mJointList.at(i);
				Joint* parentJoint = mJointList.at(i - 1);

				glm::vec4 dir = glm::normalize(parentJoint->mPos - currJoint->mPos);	//set the direction vector to place the parent joint inline

				parentJoint->mPos = currJoint->mPos - (mDistances.at(i - 1) * dir);	//set position of parent joint in the direction of the joint movement by the distance between
			}

			mJointList.at(0)->mPos = firstBasePos;		//move the beginning joint back to its root pos

			for (int i = 0; i < mJointList.size() - 1; ++i)
			{
				Joint* currJoint = mJointList.at(i);
				Joint* childJoint = mJointList.at(i + 1);

				glm::vec4 dir = glm::normalize(childJoint->mPos - currJoint->mPos);
				childJoint->mPos = currJoint->mPos + (mDistances.at(i) * dir);
			}
		}
	}

	/*
		rather than just giving a higher target, we can use a set series of target positions that only get adjusted, then
		add whatever hieght is necessary to the end effector. The targetPos can always stay the same while we take in the height of the platform
		in comparison to the height of the character
	*/
	void SolveInverseKinematicsIterWithPlat(glm::vec4 targetPos, int iterations)
	{
		if (mJointList.empty() || mEffectorDistance >= mChainLength)
		{
			return;
		}

		int iter = 0;
		if (iterations <= 0)
		{
			iter = 1;
		}

		glm::vec4 firstBasePos = mJointList.at(0)->mPos;	//record original root location

		for (int i = 0; i < iter; ++i)
		{
			if (mpPlatPos != nullptr)
			{
				mJointList.back()->mPos = targetPos + (firstBasePos + *(mpPlatPos));	//move the endEffector to the target plus the height of the platform in comparison to hieght of current foot or hand
			}
			else
			{
				mJointList.back()->mPos = targetPos;
			}

			//go through the chain from end to beginning
			for (int i = mJointList.size(); i > 1; --i)		//has to be this weird way for parents
			{
				Joint* currJoint = mJointList.at(i);
				Joint* parentJoint = mJointList.at(i - 1);

				glm::vec4 dir = glm::normalize(parentJoint->mPos - currJoint->mPos);	//set the direction vector to place the parent joint inline

				parentJoint->mPos = currJoint->mPos - (mDistances.at(i - 1) * dir);	//set position of parent joint in the direction of the joint movement by the distance between
			}

			mJointList.at(0)->mPos = firstBasePos;		//move the beginning joint back to its root pos

			for (int i = 0; i < mJointList.size() - 1; ++i)
			{
				Joint* currJoint = mJointList.at(i);
				Joint* childJoint = mJointList.at(i + 1);

				glm::vec4 dir = glm::normalize(childJoint->mPos - currJoint->mPos);
				childJoint->mPos = currJoint->mPos + (mDistances.at(i) * dir);
			}
		}
	}

	bool passMarginOfError(glm::vec4 endEffectorPos, glm::vec4 targetPos, float moe)
	{
		//check if our end joint is close enough to the target
		if (glm::length(endEffectorPos - targetPos) > moe)
		{
			return false;
		}

		return true;
	}

	std::vector<Joint*> mJointList;

	std::vector<float> mDistances;
	float mChainLength;	//total length of all joints
	float mEffectorDistance;	//distance from root to effector
	int mBEN;
	int mEEN;
	Joint* mpBaseEffector;
	Joint* mpEndEffector;
	HierarchicalPosePool* mpSkeleton;

	glm::vec4* mpPlatPos;

	/*
	ProceduralGrab(Joint* baseEffector, Joint* endEffector, HierarchicalPosePool* skeleton, int baseEffectorNode, int endEffectorNode, int length)
	{
		mpBaseEffector = baseEffector;
		mpEndEffector = endEffector;
		mLength = length;
		mBEN = baseEffectorNode;	//use to determine place in hierarchy
		mEEN = endEffectorNode;
		mpSkeleton = skeleton;
	}

	void SolveInverseKinematics(glm::vec4 targetPos)
	{
		if (mLength < 0)
		{
			return;
		}

		//get actual distance between the joints, like physical distance
		std::vector<float> distances;
		for (int i = mBEN; i < mEEN - 1; ++i)
		{
			//get node pose, get parent pos, get distance
			float dist = glm::length(mpSkeleton->getJointFromBase(i)->mPos - mpSkeleton->getJointFromBase(i + 1)->mPos);
			distances.push_back(dist);
		}

		mpEndEffector->mPos = targetPos;	//move the endEffector to the target

		//go through the 
		for (int i = mEEN; i > mLength; --i)
		{
			mpCurrentJoint 
		}

	}

	int mLength;
	int mBEN;
	int mEEN;
	Joint* mpCurrentJoint;
	Joint* mpBaseEffector;
	Joint* mpEndEffector;
	HierarchicalPosePool* mpSkeleton;
	*/
};