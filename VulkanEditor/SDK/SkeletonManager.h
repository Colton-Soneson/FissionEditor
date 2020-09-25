#pragma once
#include "AnimHierarchies.h"

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

	
	

	void calculateGlobalSpacePose()
	{
		//interpolation
		//concatenation
		//conversion
		//FK equation
	}

	int mNumberOfPoses;
	Hierarchy* mpHierarchy;
	HierarchicalPosePool* mpHierarchicalPosePool;
};

//incase later on we add more skeletons
struct SkeletonContainer
{
	std::vector<HumanoidBasic> mHumanoidBasics;
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

	SkeletonContainer getSkeletonContainer() { return mSkeletonContainer; }

private:
	SkeletonContainer mSkeletonContainer;

};