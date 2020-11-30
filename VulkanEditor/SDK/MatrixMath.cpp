#include "MatrixMath.h"

namespace mmath
{
	glm::mat4 XRotationMatrix(float theta)
	{
		glm::mat4 r;
		r[0][0] = 1;
		r[0][1] = 0;
		r[0][2] = 0;
		r[0][3] = 0;

		r[1][0] = 0;
		r[1][1] = glm::cos(theta);
		r[1][2] = glm::sin(theta);
		r[1][3] = 0;

		r[2][0] = 0;
		r[2][1] = -glm::sin(theta);
		r[2][2] = glm::cos(theta);
		r[2][3] = 0;

		r[3][0] = 0;
		r[3][1] = 0;
		r[3][2] = 0;
		r[3][3] = 1;

		return r;
	}

	glm::mat4 YRotationMatrix(float theta)
	{
		glm::mat4 r;
		r[0][0] = glm::cos(theta);
		r[0][1] = 0;
		r[0][2] = -glm::sin(theta);
		r[0][3] = 0;

		r[1][0] = 0;
		r[1][1] = 1;
		r[1][2] = 0;
		r[1][3] = 0;

		r[2][0] = glm::sin(theta);
		r[2][1] = 0;
		r[2][2] = glm::cos(theta);
		r[2][3] = 0;

		r[3][0] = 0;
		r[3][1] = 0;
		r[3][2] = 0;
		r[3][3] = 1;

		return r;
	}

	glm::mat4 ZRotationMatrix(float theta)
	{
		glm::mat4 r;
		r[0][0] = glm::cos(theta);
		r[0][1] = glm::sin(theta);
		r[0][2] = 0;
		r[0][3] = 0;

		r[1][0] = -glm::sin(theta);
		r[1][1] = glm::cos(theta);
		r[1][2] = 0;
		r[1][3] = 0;

		r[2][0] = 0;
		r[2][1] = 0;
		r[2][2] = 1;
		r[2][3] = 0;

		r[3][0] = 0;
		r[3][1] = 0;
		r[3][2] = 0;
		r[3][3] = 1;

		return r;
	}
}

glm::vec3 MatrixMath::extractTranslation(glm::mat4 matrix)
{
	glm::vec3 temp = glm::vec3(0.0);
	
	temp.x = matrix[3][0];
	temp.y = matrix[3][1]; 
	temp.z = matrix[3][2];

	return temp;
}

glm::vec3 MatrixMath::extractScale(glm::mat4 matrix)
{
	glm::vec3 temp;

	temp = glm::vec3(matrix[0][0], matrix[0][1], matrix[0][2]);
	float scaleX = glm::sqrt((temp.x * temp.x + temp.y * temp.y + temp.z * temp.z));

	temp = glm::vec3(matrix[1][0], matrix[1][1], matrix[1][2]);
	float scaleY = glm::sqrt((temp.x * temp.x + temp.y * temp.y + temp.z * temp.z));
	
	temp = glm::vec3(matrix[2][0], matrix[2][1], matrix[2][2]);
	float scaleZ = glm::sqrt((temp.x * temp.x + temp.y * temp.y + temp.z * temp.z));

	return glm::vec3(scaleX, scaleY, scaleZ);
}

Quaternion MatrixMath::exractQuaternion(glm::mat3 Rot_matrix)
{
	//https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf
	//a more efficient way of getting a quaternion out of a rot matrix

	float k;
	Quaternion result;

	if (Rot_matrix[2][2] < 0)	// |(x,y)| bigger than |(z,w)|
	{
		if (Rot_matrix[0][0] > Rot_matrix[1][1])	//|x| bigger than |y|
		{
			//use x form
			k = 1 + Rot_matrix[0][0] - Rot_matrix[1][1] - Rot_matrix[2][2];
			result.setVals(k, Rot_matrix[0][1] + Rot_matrix[1][0], Rot_matrix[2][0] + Rot_matrix[0][2], Rot_matrix[1][2] - Rot_matrix[2][1]);
		}
		else
		{
			//use y form
			k = 1 - Rot_matrix[0][0] + Rot_matrix[1][1] - Rot_matrix[2][2];
			result.setVals(Rot_matrix[0][1] + Rot_matrix[1][0], k, Rot_matrix[1][2] + Rot_matrix[2][1], Rot_matrix[2][0] - Rot_matrix[0][2]);
		}
	}
	else
	{
		if (Rot_matrix[0][0] < -Rot_matrix[1][1])	//|z| bigger than |w|
		{
			//use z form
			k = 1 - Rot_matrix[0][0] - Rot_matrix[1][1] + Rot_matrix[2][2];
			result.setVals(Rot_matrix[2][0] + Rot_matrix[0][2], Rot_matrix[1][2] + Rot_matrix[2][1], k, Rot_matrix[0][1] - Rot_matrix[1][0]);
		}
		else
		{
			//use w form
			k = 1 + Rot_matrix[0][0] + Rot_matrix[1][1] + Rot_matrix[2][2];
			result.setVals(Rot_matrix[1][2] - Rot_matrix[2][1], Rot_matrix[2][0] - Rot_matrix[0][2], Rot_matrix[0][1] - Rot_matrix[1][0], k);
		}
	}

	result *= (float)(0.5 / sqrt(k));

	return result;
}

glm::vec3 MatrixMath::quaternionToEuler(Quaternion quat)
{
	//https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

	glm::vec3 result;
	
	//roll
	float sinRcosP = 2 * (quat.w * quat.i + quat.j * quat.k);
	float cosRcosP = 1 - 2 * (quat.i * quat.i + quat.j * quat.j);
	result.x = std::atan2(sinRcosP, cosRcosP);

	//pitch
	float sinP = 2 * (quat.w * quat.j - quat.k * quat.i);
	if (std::abs(sinP) >= 1)
	{
		result.y = std::copysign(glm::pi<float>() / 2, sinP);
	}
	else
	{
		result.y = std::asin(sinP);
	}

	float sinYcosP = 2 * (quat.w * quat.k + quat.i * quat.j);
	float cosYcosP = 1 - 2 * (quat.j * quat.j + quat.k * quat.k);
	result.z = std::atan2(sinYcosP, cosYcosP);

	return result;
}

glm::vec3 MatrixMath::extractEulerRotation(glm::mat4 matrix)
{
	glm::mat4 temp = matrix;

	glm::vec3 ES = extractScale(temp);

	//get scale and position out of it
	temp[0][0] = temp[0][0] / ES.x;
	temp[1][1] = temp[1][1] / ES.y;
	temp[2][2] = temp[2][2] / ES.z;


	glm::mat3 rotMat;
	
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			rotMat[i][j] = temp[i][j];
		}
	}

	glm::vec3 result = quaternionToEuler(exractQuaternion(rotMat));

	return result;
}