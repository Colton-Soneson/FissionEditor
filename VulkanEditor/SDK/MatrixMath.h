#pragma once
#define GLM_FORCE_RADIANS	//makes it so shit like rotate uses radians instead of eulerAngles
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES		//This is for solving contiguous memory for you (may have problems with nested stuff)
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		//depth testing, configure matrix to range of 0 to 1 instead of -1 to 1
#define GLM_ENABLE_EXPERIMENTAL		
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>		//rotate, lookAt, perspective
#include <glm/gtc/constants.hpp>			//pi 

#include <math.h>
#include <vector>
#include <utility>
#include <string>
#include <map>
#include <tuple>

struct triangle2D
{
	std::pair<float, float> mV0;
	std::pair<float, float> mV1;
	std::pair<float, float> mV2;
};

struct Quaternion
{
	float i, j, k = 0;
	double w;

	void setVals(float _i, float _j, float _k, float _w)
	{
		i = _i;
		j = _j;
		k = _k;
		w = _w;
	}

	Quaternion operator *= (float t)
	{
		Quaternion result;
		result.i = i * t;
		result.j = j * t;
		result.k = k * t;
		result.w = w * t;
		return result;
	}
};

namespace mmath
{
	//theta in this case is x degrees
	glm::mat4 XRotationMatrix(float theta);

	//theta in this case is y degrees
	glm::mat4 YRotationMatrix(float theta);

	//theta in this case is z degrees
	glm::mat4 ZRotationMatrix(float theta);
}

class MatrixMath
{
public:
	MatrixMath() {};
	~MatrixMath() {};

	glm::vec3 extractTranslation(glm::mat4 TRS_matrix);
	glm::vec3 extractScale(glm::mat4 TRS_matrix);
	glm::vec3 extractEulerRotation(glm::mat4 TRS_matrix);
	Quaternion exractQuaternion(glm::mat3 Rot_matrix);		//this takes in the rotation matrix (no scale from TRS)

	glm::vec3 quaternionToEuler(Quaternion quat);
	
};
