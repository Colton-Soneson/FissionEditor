#pragma once
#include "AnimHierarchies.h"
#include <fstream>
#include <sstream>

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

		mpHierarchicalPosePool->addToHierarchicalPoses(mNumberOfPoses);
		mNumberOfPoses++;
		
	}

	//use addKeyPose to create 3 more poses that work from base pose
	void addKeyPose()
	{
		//taking in the base pose, create modifications
		HierarchicalPose tempH = mpHierarchicalPosePool->getHierarchicalPoses().at(0);	
		Joint tempJ;
		tempJ.copy(&tempJ, tempH.mHierarchicalJoints.at(5));
		tempJ.setPosition(glm::vec4(0, +5, 0, 0));
		tempH.mHierarchicalJoints.at(5) = &tempJ;
		mpHierarchicalPosePool->addToHierarchicalPoses(tempH);

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

		if (htr.is_open())
		{
			
			while (std::getline(htr, line))
			{
				std::istringstream iss(line);
				std::string word;

				if (mStages.at(0) == line)
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
				else if (mStages.at(1) == line)
				{
					do
					{
						iss >> word;	//set word of line

						if (word[0] == '#')	//if we encounter the comment symbol ditch the word parser and therefore the line
						{
							break;
						}

						mChildren.push_back(word);

						iss >> word;

						mParent.push_back(word);

					} while (iss);

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

	//stages into file
	//	specific to HTR file
	std::vector<std::string> mStages = {"[Header]", "[SegmentNames&Hierarchy]", "[BasePosition]", "[EndOfFile]"};

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

	//base positions



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
	SkeletonManager()
	{

	}

	~SkeletonManager() {};

	void update();
	void createHumanoidBasic();
	void createHTRSkeleton();

	SkeletonContainer getSkeletonContainer() { return mSkeletonContainer; }

private:
	SkeletonContainer mSkeletonContainer;

};