#pragma once
#include "MatrixMath.h"

#include <vector>
#include <utility>
#include <string>


//structure for JointEulerOrder and JointChannel was provided for by Dan Buckstein
enum JointEulerOrder
{
	jointEulerOrder_xyz,
	jointEulerOrder_yzx,
	jointEulerOrder_zxy,
	jointEulerOrder_yxz,
	jointEulerOrder_xzy,
	jointEulerOrder_zyx,
};

enum JointChannel
{
	// identity (no channels)
	jointChannel_none,

	// orientation
	jointChannel_orient_x = 0x0001,
	jointChannel_orient_y = 0x0002,
	jointChannel_orient_z = 0x0004,
	jointChannel_orient_xy = jointChannel_orient_x | jointChannel_orient_y,
	jointChannel_orient_yz = jointChannel_orient_y | jointChannel_orient_z,
	jointChannel_orient_zx = jointChannel_orient_z | jointChannel_orient_x,
	jointChannel_orient_xyz = jointChannel_orient_xy | jointChannel_orient_z,

	// scale
	jointChannel_scale_x = 0x0010,
	jointChannel_scale_y = 0x0020,
	jointChannel_scale_z = 0x0040,
	jointChannel_scale_xy = jointChannel_scale_x | jointChannel_scale_y,
	jointChannel_scale_yz = jointChannel_scale_y | jointChannel_scale_z,
	jointChannel_scale_zx = jointChannel_scale_z | jointChannel_scale_x,
	jointChannel_scale_xyz = jointChannel_scale_xy | jointChannel_scale_z,

	// translation
	jointChannel_translate_x = 0x0100,
	jointChannel_translate_y = 0x0200,
	jointChannel_translate_z = 0x0400,
	jointChannel_translate_xy = jointChannel_translate_x | jointChannel_translate_y,
	jointChannel_translate_yz = jointChannel_translate_y | jointChannel_translate_z,
	jointChannel_translate_zx = jointChannel_translate_z | jointChannel_translate_x,
	jointChannel_translate_xyz = jointChannel_translate_xy | jointChannel_translate_z,
};


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

	void reset()
	{
		mTransform = glm::mat4(1.0);
		mPos = glm::vec4(0.0);
		mRot = glm::vec4(0.0);
		mScale = glm::vec4(1.0);
	}

	void copy(Joint* out, Joint* in)
	{
		if (out && in)
		{
			out->setPosition(in->mPos);
			out->setRotation(in->mRot);
			out->setScale(in->mScale);
		}
	}

	void concatenate(Joint* out, Joint* lhs, Joint* rhs)
	{
		if (out && lhs && rhs)
		{
			out->setPosition(lhs->mPos + rhs->mPos);
			out->setRotation(lhs->mRot + rhs->mRot);
			out->setScale(lhs->mScale * rhs->mScale);
		}
	}

	void lerp(Joint* out, Joint* j0, Joint* j1, float time)
	{
		if (out && j0 && j1)
		{
			out->setPosition(glm::mix(j0->mPos, j1->mPos, time));
			out->setRotation(glm::mix(j0->mRot, j1->mRot, time));
			out->setScale(glm::mix(j0->mScale, j1->mScale, time));
		}
	}

	glm::mat4 convertToMat(Joint* in, JointChannel jc, JointEulerOrder jeo)
	{
		glm::mat4 temp;

		if (in)
		{

		}


		return temp;
	}


	void setTransform(glm::mat4 t) { mTransform = t; }
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

	void addToHierarchicalPoses(int poseNumber)
	{
		//pose number is the number of the pose, A / T pose would be 0, any poses after that would be 1, 2, 3....
		int indexInSpatialPool = poseNumber * mHierarchy->getNumberOfNodes();
		std::vector<bool> visited(mHierarchy->getNumberOfNodes(), false);
		HierarchicalPose hp;

		for (int i = indexInSpatialPool; i < indexInSpatialPool + mHierarchy->getNumberOfNodes(); ++i)	//go through each spatial pose by the number of nodes in hierarchy
		{
			hp.mHierarchicalJoints.push_back(&mSpatialPosePool[i]);
		}

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
	/*ForwardKinematics(HierarchicalState* state)
	{
		mHState = state;
	}*/

	void fkAlgorithm(HierarchicalState* mHState)
	{
		//for each node in the hierarchy associated with mHState
		//	if node is NOT root node
		//		nodes global-space pose transform is the product of its parents global-space pose transform, and its own local-space pose transform
		//	else the node IS root node
		//		nodes global-space pose is its own local-space pose transform

		int numNodes = mHState->mHierarchy->getNumberOfNodes();

		for (int i = 0; i <= numNodes; ++i)	//CHECK IF  LESS EQUAL IS RIGHT OR JUST LESS
		{
			int nodeParentIndex = mHState->mHierarchy->getNodes().at(i).mParentIndex;
			int nodeIndex = mHState->mHierarchy->getNodes().at(i).mIndex;
			
			if (nodeIndex != nodeParentIndex)	//this will only happen to the root node, it would be 0 0
			{
				glm::mat4 childLocalMat = mHState->mLocalTransformPose.mHierarchicalJoints.at(nodeIndex)->mTransform;
				glm::mat4 parentGlobalMat = mHState->mGlobalTransformPose.mHierarchicalJoints.at(nodeParentIndex)->mTransform;

				mHState->mGlobalTransformPose.mHierarchicalJoints.at(i)->setTransform(parentGlobalMat * childLocalMat);
			}
			else
			{
				mHState->mGlobalTransformPose.mHierarchicalJoints.at(i) = mHState->mLocalTransformPose.mHierarchicalJoints.at(i);
			}
		}
		
	}

	//HierarchicalState* mHState;
};

