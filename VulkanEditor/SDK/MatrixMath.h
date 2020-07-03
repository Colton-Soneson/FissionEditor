#pragma once

#define GLM_FORCE_RADIANS	//makes it so shit like rotate uses radians instead of eulerAngles
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES		//This is for solving contiguous memory for you (may have problems with nested stuff)
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		//depth testing, configure matrix to range of 0 to 1 instead of -1 to 1
#define GLM_ENABLE_EXPERIMENTAL		
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>		//rotate, lookAt, perspective

#include <math.h>

class MatrixMath
{
public:
	MatrixMath() {};
	~MatrixMath() {};

	glm::vec3 extractTranslation(glm::mat4 matrix);
	glm::vec3 extractScale(glm::mat4 matrix);
	glm::vec3 extractEulerRotation(glm::mat4 matrix);
	
};
