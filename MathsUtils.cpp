#include "MathsUtils.h"

void Utils::MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
{
	/* 
	This code multiplies a matrix with a vector, we're able to do this because the number 
	of columns in the matrix is equal to the number of rows in the vector 
	*/
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];

	// this makes sure the correct perspective scaling is applied to each point
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
	if (w != 0.0f)
	{
		o.x /= w; o.y /= w; o.z /= w;
	}
}
// This function defines the projection matrix
Utils::mat4x4 Utils::DefineProjectionMatrix(float ScreenHeight, float ScreenWidth) 
{
	/* 
	* This is the projection matrix, it's used to convert 3D coordinates to 2D coordinates.
	* It's one of the most fundamental components of this project.
	* It's fully explained in my design document.
	*/

	Utils::mat4x4 projectionMatrix;

	if (ScreenHeight == 0 || ScreenWidth == 0)
	{
	//	cout << "Error: Screen height or width has been set to 0" << endl;
		return projectionMatrix;
	}

	float nearPlane = 10.0f;
	float farPlane = 100.0f;
	float fov = 40.0f;
	float aspectRatio = ScreenHeight / ScreenWidth;
	// perform trignometric calulations to calulate the field of view
	float fovRad = 1.0f / tanf(fov * 0.5f / 180 * 3.14159f);

	// assign matrix elements
	projectionMatrix.m[0][0] = aspectRatio * fovRad;
	projectionMatrix.m[1][1] = fovRad;
	projectionMatrix.m[2][2] = farPlane / (farPlane - nearPlane);
	projectionMatrix.m[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
	projectionMatrix.m[2][3] = 1.0f;
	projectionMatrix.m[3][3] = 0.0f;

	return projectionMatrix;
}

// This function calculates the factorial of a number
int Utils::factorial(int input) 
{
	int value = 1;

	// Mathematically the factorial of 0 and 1 is 1
	if (input == 0 || input == 1)
		return 1;

	for (int i = 1; i <= input; i++)
	{
		value *= i;
	}

	return value;
}

// This function multiplies two matrices together
Utils::mat4x4 Utils::MultiplyMatrix(mat4x4& i, mat4x4& o)
{
	mat4x4 input;

	for (int rows = 0; rows <= 3; rows++)
	{
		float total = 0;

		for (int column = 0; column <= 3; column++)
		{
			for (int step = 0; step <= 3; step++)
			{
				total += i.m[step][rows] * o.m[column][step];
			}
			input.m[column][rows] = total;
			total = 0;
		}
	}
	return input;
}

Utils::mat4x4 Utils::MatRotationZ(float zRot)
{
	mat4x4 matRotZResult;

	/* This is the rotation matrix for the Z axis, it rotates the object around the Z axis
	* the matrix is defined as follows:
	[ cosθ   -sinθ    0 ]
	[ sinθ    cosθ    0 ]
	[  0       0      1 ]
	* theta in this matrix is the angle of rotation around the Z axis
	*/

	matRotZResult.m[0][0] = cosf(0);
	matRotZResult.m[1][0] = -sinf(0);
	matRotZResult.m[2][0] = 0;
	matRotZResult.m[0][1] = sinf(0);
	matRotZResult.m[1][1] = cosf(0);
	matRotZResult.m[2][1] = 0;
	matRotZResult.m[0][2] = 0;
	matRotZResult.m[1][2] = 0;
	matRotZResult.m[2][2] = 1;
	matRotZResult.m[3][3] = 1;
	return matRotZResult;
}
Utils::mat4x4 Utils::MatRotationY(float yRot)
{
	mat4x4 matRotYResult;

	/* This is the rotation matrix for the Y axis, it rotates the object around the Y axis
	* the matrix is defined as follows:
	[ cosθ    0    sinθ ]
	[ 0       1      0  ]
	[ -sinθ   0    cosθ ]
	* theta in this equation is the angle of rotation around the Y axis
	*/

	matRotYResult.m[0][0] = cosf(yRot);
	matRotYResult.m[0][2] = -sinf(yRot);
	matRotYResult.m[1][0] = 0;
	matRotYResult.m[1][1] = 1;
	matRotYResult.m[2][0] = sinf(yRot);
	matRotYResult.m[2][2] = cosf(yRot);
	matRotYResult.m[3][3] = 1;
	return matRotYResult;
} 
Utils::mat4x4 Utils::MatRotationX(float xRot)
{
	mat4x4 matRotXResult;

	/* This is the rotation matrix for the X axis, it rotates the object around the X axis 
	* the matrix is defined as follows:         
	[ 1    0      0    ]
    [ 0  cosθ   -sinθ  ]
    [ 0  sinθ    cosθ  ]
	* theta in this equation is the angle of rotation around the X axis
	*/

	matRotXResult.m[0][0] = 1;
	matRotXResult.m[1][0] = 0;
	matRotXResult.m[2][0] = 0;
	matRotXResult.m[0][1] = 0;
	matRotXResult.m[1][1] = cosf(xRot);
	matRotXResult.m[2][1] = -sinf(xRot);
	matRotXResult.m[0][2] = 0;
	matRotXResult.m[1][2] = sinf(xRot);
	matRotXResult.m[2][2] = cosf(xRot);
	matRotXResult.m[3][3] = 1;
	return matRotXResult;
}
