#include "MatrixMath.h"

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

glm::vec3 MatrixMath::extractEulerRotation(glm::mat4 matrix)
{
	glm::mat4 temp = matrix;

	glm::vec3 ES = extractScale(temp);

	//get scale and position out of it
	temp[0][0] = temp[0][0] / ES.x;
	temp[1][1] = temp[1][1] / ES.y;
	temp[2][2] = temp[2][2] / ES.z;
	temp[3][0] = 0;
	temp[3][1] = 0;
	temp[3][2] = 0;

	//http://danceswithcode.net/engineeringnotes/rotations_in_3d/rotations_in_3d_part2.html

	//float sinY = -temp[2][0];
	//float cosY = glm::sqrt(1 - sinY * sinY);

	 glm::vec3 result;

	////float sy = sqrt(temp[0][0] * temp[0][0] + temp[1][0] * temp[1][0]);

	////if (sy < 1e-6)
	//if (glm::abs(cosY) > FLT_EPSILON)
	//{
	//	float sinX = temp[2][1] / cosY;
	//	float cosX = temp[2][2] / cosY;
	//	float sinZ = temp[1][0] / cosY;
	//	float cosZ = temp[0][0] / cosY;

	//	result.x = (atan2(sinX, cosX)) * 180 / 3.14159;
	//	result.y = (atan2(sinY, cosY)) * 180 / 3.14159;
	//	result.z = (atan2(sinZ, cosZ)) * 180 / 3.14159;

	//	//result.x = atan2(-temp[1][2], temp[1][1]) * 180 / 3.14159;
	//	//result.y = atan2(-temp[2][0], sy) * 180 / 3.14159;
	//	//result.z = 0;
	//}
	//else
	//{
	//	float sinX = -temp[1][2];
	//	float cosX = temp[1][1];
	//	float sinZ = 0;
	//	float cosZ = 1;

	//	result.x = (atan2(sinX, cosX)) * 180 / 3.14159;
	//	result.y = (atan2(sinY, cosY)) * 180 / 3.14159;
	//	result.z = (atan2(sinZ, cosZ)) * 180 / 3.14159;

	//	//result.x = atan2(temp[2][1], temp[2][2]) * 180 / 3.14159;
	//	//result.y = atan2(-temp[2][0], sy) * 180 / 3.14159;
	//	//result.z = atan2(temp[1][0], temp[0][0]) * 180 / 3.14159;
	//}

	return result;
}