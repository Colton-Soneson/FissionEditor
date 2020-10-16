#pragma once
#include "MatrixMath.h"



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

enum HierarchicalStateMode
{
	BASE_ONLY,
	MANUAL_FLIP,
	AUTO_CC_FLIP,
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
		mScale = glm::vec4(1.0, 1.0, 1.0, 0.0);
	}

	Joint(glm::mat4 transform, glm::vec4 position, glm::vec4 eulerRotation, glm::vec4 scale)
	{
		mTransform = transform;
		mPos = position;
		mRot = eulerRotation;
		mScale = scale;
	}

	Joint(glm::vec4 position, glm::vec4 eulerRotation, glm::vec4 scale)
	{
		mTransform = glm::mat4(1.0);
		mPos = position;
		mRot = eulerRotation;
		mScale = scale;
	}

	void reset()
	{
		mTransform = glm::mat4(1.0);
		mPos = glm::vec4(0.0);
		mRot = glm::vec4(0.0);
		mScale = glm::vec4(1.0, 1.0, 1.0, 0.0);
	}

	

	void convertToMat(Joint* in, JointChannel jc, JointEulerOrder jeo)
	{
		//the states local pose is both the source and the target
		//this seems to also filter our conversion results

		glm::mat4 tempLocal;
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

	std::vector<HierarchicalPose> getHierarchicalPoses() { return mHierarchicalPoses; }

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
		resetAll();
	}

	void resetAll()
	{
		for (int i = 0; i < mHierarchy->getNumberOfNodes(); ++i)
		{
			mAnimatedPose.mHierarchicalJoints.at(i)->reset();
			mLocalTransformPose.mHierarchicalJoints.at(i)->reset();
			mGlobalTransformPose.mHierarchicalJoints.at(i)->reset();
		}
	}

	

	HierarchicalStateMode mHSM;

	Hierarchy* mHierarchy;						//GIVEN THIS HIERARCHY, allocate and reset the poses below
	HierarchicalPose mAnimatedPose;				//current animated pose at the current time
	HierarchicalPose mLocalTransformPose;		//represents the node's transformation relative to parent
	HierarchicalPose mGlobalTransformPose;		//represents the node's transformation relative to the root's parent space

};


struct ForwardKinematics
{

	void fkAlgorithm(HierarchicalState* mHState)
	{
		//THIS IS THE VERSION FOR A DEPTH FIRST BASED TREE

		//for each node in the hierarchy associated with mHState
		//	if node is NOT root node
		//		nodes global-space pose transform is the product of its parents global-space pose transform, and its own local-space pose transform
		//	else the node IS root node
		//		nodes global-space pose is its own local-space pose transform

		int numNodes = mHState->mHierarchy->getNumberOfNodes();

		for (int i = 0; i < numNodes; ++i)	//CHECK IF  LESS EQUAL IS RIGHT OR JUST LESS
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

struct BlendOperations
{
public:
	Joint* identity()
	{
		return new Joint(glm::vec4(0.0), glm::vec4(0.0), glm::vec4(1.0,1.0,1.0,0.0));
	}

	Joint* construct(Joint* out, glm::vec4 _pos, glm::vec4 _rot, glm::vec4 _scale)
	{
		out->mPos = _pos;
		out->mRot = _rot;
		out->mScale = _scale;
		return out;
	}

	Joint* copy(Joint* out, Joint* in)
	{
		out->setPosition(in->mPos);
		out->setRotation(in->mRot);
		out->setScale(in->mScale);
		return out;
	}

	Joint* invert(Joint* out)
	{
		out->mPos *= -1.0;
		out->mRot *= -1.0;
		out->mScale.x = 1.0 / out->mScale.x;
		out->mScale.y = 1.0 / out->mScale.y;
		out->mScale.z = 1.0 / out->mScale.z;

		return out;
	}

	Joint* merge(Joint* lhs, Joint* rhs)
	{
		lhs->setPosition(lhs->mPos + rhs->mPos);
		lhs->setRotation(lhs->mRot + rhs->mRot);
		lhs->setScale(lhs->mScale * rhs->mScale);
		return lhs;
	}

	Joint* nearest(Joint* j0, Joint* j1, float u)
	{
		if (u < 0.5f) { return j0; }
		else { return j1; }
	}

	Joint* mix(Joint* j0, Joint* j1, float u)
	{
		//out->setPosition(glm::mix(j0->mPos, j1->mPos, time));
		//out->setRotation(glm::mix(j0->mRot, j1->mRot, time));
		//out->setScale(glm::mix(j0->mScale, j1->mScale, time));
		
		Joint* out = new Joint();

		out->mPos = j0->mPos + u * (j1->mPos - j0->mPos);
		out->mRot = j0->mRot + u * (j1->mRot - j0->mRot);
		out->mScale = j0->mScale + u * (j1->mScale - j0->mScale);
				
		return out;
	}

	Joint* cubic(Joint* jPreInit, Joint* jInit, Joint* jTerm, Joint* jPost, float u)
	{
		glm::mat4 posMat = glm::mat4(jPreInit->mPos, jInit->mPos, jTerm->mPos, jPost->mPos);
		glm::mat4 rotMat = glm::mat4(jPreInit->mRot, jInit->mRot, jTerm->mRot, jPost->mRot);
		glm::mat4 scaleMat = glm::mat4(jPreInit->mScale, jInit->mScale, jTerm->mScale, jPost->mScale);

		const glm::mat4 k = glm::mat4(0.0, 2.0, 0.0, 0.0,
									1.0, 0.0, 1.0, 0.0,
									2.0, -5.0, 4.0, -1.0,
									-1.0, 3.0, -3.0, 1.0);

		float t0 = 0.5;
		float t1 = t0 * u;	//basically u
		float t2 = t1 * u;	//basically u^2
		float t3 = t2 * u;	//basically u^3

		glm::vec4 t = glm::vec4(t0, t1, t2, t3);

		Joint* out = new Joint();

		out->mPos = (posMat * (k * t));
		out->mRot = (rotMat * (k * t));
		out->mScale = (scaleMat * (k * t));

		return out;
	}


	//--------------------ADVANCED----------------------
	
	Joint* split(Joint* j0, Joint* j1)
	{
		return merge(j0, invert(j1));
	}

	Joint* scale(Joint* j0, float u)
	{
		return mix(identity(), j0, u);
	}

	Joint* tri(Joint* j0, Joint* j1, Joint* j2, float u0, float u1)
	{
		//kinda like a oneMinus
		float u = 1 - (u0 - u1);

		return merge(merge(scale(j0, u), scale(j1, u0)), scale(j2, u1));
	}

	Joint* binearest(Joint* j00, Joint* j01, Joint* j10, Joint* j11, float u0, float u1, float u)
	{
		return nearest(nearest(j00, j01, u0), nearest(j10, j11, u1), u);
	}

	Joint* bilerp(Joint* out, Joint* j00, Joint* j01, Joint* j10, Joint* j11, float u0, float u1, float u)
	{
		return mix(mix(j00, j01, u0), mix(j10, j11, u1), u);
	}

	Joint* bicubic(Joint* out, Joint* jPreInit0, Joint* jInit0, Joint* jTerm0, Joint* jPost0,
		Joint* jPreInit1, Joint* jInit1, Joint* jTerm1, Joint* jPost1,
		Joint* jPreInit2, Joint* jInit2, Joint* jTerm2, Joint* jPost2,
		Joint* jPreInit3, Joint* jInit3, Joint* jTerm3, Joint* jPost3,
		float u0, float u1, float u2, float u3, float u)
	{
		return cubic(cubic(jPreInit0, jInit0, jTerm0, jPost0, u0), cubic(jPreInit1, jInit1, jTerm1, jPost1, u1),
			cubic(jPreInit2, jInit2, jTerm2, jPost2, u2), cubic(jPreInit3, jInit3, jTerm3, jPost3, u3), u);
	}

	//bonuses

	//j0 + (j1 - j0)u
	Joint* compMix(Joint* j0, Joint* j1, float u)
	{
		//use the equations we just developed
		return merge(j0, scale(split(j1, j0), u));
	}

	//DELAUNAY
	//triangle collision brought by https://www.gamedev.net/forums/topic.asp?topic_id=295943
	float sign(std::pair<float, float> p0, std::pair<float, float> p1, std::pair<float, float> p2)
	{
		return (p0.first - p2.first) * (p1.second - p2.second) - (p1.first - p2.first) * (p0.second - p2.second);
	}

	bool trigangularCollision(std::pair<float, float> p0, std::pair<float, float> p1, std::pair<float, float> p2, std::pair<float, float> pC)
	{
		float deriv0 = sign(pC, p0, p1);
		float deriv1 = sign(pC, p1, p2);
		float deriv2 = sign(pC, p2, p1);

		return !(((deriv0 < 0) || (deriv1 < 0) || (deriv2 < 0)) && 
					((deriv0 > 0) || (deriv1 > 0) || (deriv2 > 0)));
	}

	Joint* Delaunay(std::vector<triangle2D> setOfTriangles, std::map<std::pair<float, float>, Joint*> jointGridPoints, std::pair<float, float> point)
	{
		//we need to find the three joints grabbed from jointGridPoints forming the triangle that point is in of the setOfTrianglePoints
		Joint* j0 = new Joint();
		Joint* j1 = new Joint();
		Joint* j2 = new Joint();

		for (int i = 0; i < setOfTriangles.size() - 1; ++i)
		{
			//check to see if we are in a triangle with this point
			if (trigangularCollision(setOfTriangles.at(i).mV0, setOfTriangles.at(i).mV0, setOfTriangles.at(i).mV0, point) == true)
			{
				copy(j0, jointGridPoints.at(setOfTriangles.at(i).mV0));
				copy(j1, jointGridPoints.at(setOfTriangles.at(i).mV1));
				copy(j2, jointGridPoints.at(setOfTriangles.at(i).mV2));
			}
		}


		return tri(j0, j1, j2, point.first, point.second);
	}

};

