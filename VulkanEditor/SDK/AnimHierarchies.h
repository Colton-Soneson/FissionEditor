#pragma once
#include "MatrixMath.h"

#include <vector>
#include <utility>
#include <string>



struct Node
{
	Node()
	{
		mName = "DEFAULT";
		mIndex = 0;
		mParentIndex = 0;
	}

	Node(std::string name, int index, int parentIndex)
	{
		mName = name;
		mIndex = index;
		mParentIndex = parentIndex;
	}

	std::string mName;
	int mIndex;				//index in hierarchy
	int mParentIndex;

	//std::pair<int, int> rowCol;	//level based parent-child, setup as (#Row, #leafOnBranch) 
	//std::vector<node*> children;
};

//STORAGE ITEM: stores joints in order by depth on how they connect
//PURPOSE:	Describes how poses are related
struct Hierarchy
{
	Hierarchy()
	{
		mNumNodes = 0;
	}

	void addNode(Node n) 
	{
		mNumNodes++;
		mNodes.push_back(n);
	}

	//void refactorHierarchy();	//ADD THIS: just incase they didnt load in the nodes correctly, reorganize the mNodes vector based on index and parent

	int getNumberOfNodes() { return mNumNodes; }
	std::vector<Node> getNodes() { return mNodes; }

private:
	std::vector<Node> mNodes;	//this is loaded and organized 
	int mNumNodes;
};

//contains transform, pos, rot, and scale data
//ALL RELATIVE TO THE PARENT SPACE
struct Joint
{

	Joint()
	{
		mTransform = glm::mat4(1.0);	//this is how glm creates identity matrix
		mPos = glm::vec4(0.0);
		mRot = glm::vec4(0.0);
		mScale = glm::vec4(1.0);
	}

	Joint(glm::mat4 transform, glm::vec4 position, glm::vec4 eulerRotation, glm::vec4 scale)
	{
		mTransform = transform;
		mPos = position;
		mRot = eulerRotation;
		mScale = scale;
	}

	void setPosition(glm::vec4 pos) { mPos = pos; }
	void setRotation(glm::vec4 rot) { mRot = rot; }
	void setScale(glm::vec4 scale) { mScale = scale; }

	//this could all just be organized by uint16_t
	glm::mat4 mTransform;
	glm::vec4 mPos;
	glm::vec4 mRot;
	glm::vec4 mScale;
};

//wrapper for all joints of a single pose, represents spatial descriptions of heirarchy at a point in time
struct HierarchicalPose
{
	//given the hierarchy, spit out the spatial pose
	//describes the relationship BETWEEN the nodes

	//composed of individual node poses
	std::vector<Joint*> mHierarchicalJoints;

};

//PURPOSE: describes where the poses themselves are in space
struct HierarchicalPosePool
{

	HierarchicalPosePool(Hierarchy* h)
	{
		mHierarchy = h;
	}

	void addToSpatialPosePool(Joint j)
	{
		mSpatialPosePool.push_back(j);
	}

	void addToHierarchicalPoses(HierarchicalPose hp)
	{
		mHierarchicalPoses.push_back(hp);
	}

private:
	Hierarchy* mHierarchy;								//Reference to hierarchy associated with this spatial data 
	std::vector<Joint> mSpatialPosePool;				//contains ALL individual node poses
	std::vector<HierarchicalPose> mHierarchicalPoses;	//organizes above into a pose for the whole hierarchy at the same time
														//	its like the full skeleton for n amount of poses (Ph x n)
														//	references a subset of spacial poses
};

//contains a few named poses
//this is an interface that allows for ease of access
struct HierarchicalState
{
	HierarchicalState(Hierarchy* h)
	{
		mHierarchy = h;		
	}

	Hierarchy* mHierarchy;						//GIVEN THIS HIERARCHY, allocate and reset the poses below
	HierarchicalPose mAnimatedPose;				//current animated pose at the current time
	HierarchicalPose mLocalTransformPose;		//represents the node's transformation relative to parent
	HierarchicalPose mGlobalTransformPose;		//represents the node's transformation relative to the root's parent space

};


struct ForwardKinematics
{
	ForwardKinematics(HierarchicalState* state)
	{
		mHState = state;
	}

	void fkAlgorithm()
	{
		//for each node in the hierarchy associated with mHState
		//	if node is NOT root node
		//		nodes global-space pose transform is the product of its parents global-space pose transform, and its own local-space pose transform
		//	else the node IS root node
		//		nodes global-space pose is its own local-space pose transform

	}

	HierarchicalState* mHState;
};

