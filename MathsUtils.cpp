#include "MathsUtils.h"

void Utils::MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
{
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f)
	{
		o.x /= w; o.y /= w; o.z /= w;
	}
}

Utils::mat4x4 Utils::DefineProjectionMatrix(float ScreenHeight, float ScreenWidth) 
{
	Utils::mat4x4 temp;

	float nearPlane = 10.0f;
	float farPlane = 100.0f;
	float fov = 40.0f;
	float aspectRatio = (float)ScreenHeight / (float)ScreenWidth;
	float fovRad = 1.0f / tanf(fov * 0.5f / 180 * 3.14159f);

	temp.m[0][0] = aspectRatio * fovRad;
	temp.m[1][1] = fovRad;
	temp.m[2][2] = farPlane / (farPlane - nearPlane);
	temp.m[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
	temp.m[2][3] = 1.0f;
	temp.m[3][3] = 0.0f;

	return temp;
}

int Utils::factorial(int input) 
{
	int value = 1;

	if (input == 0 || input == 1)
		return 1;

	for (int i = 1; i <= input; i++)
	{
		value *= i;
	}

	return value;
}


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

Utils::mat4x4 Utils::matRotZ(float zRot)
{
	mat4x4 temp;

	temp.m[0][0] = cosf(0);
	temp.m[1][0] = -sinf(0);
	temp.m[2][0] = 0;
	temp.m[0][1] = sinf(0);
	temp.m[1][1] = cosf(0);
	temp.m[2][1] = 0;
	temp.m[0][2] = 0;
	temp.m[1][2] = 0;
	temp.m[2][2] = 1;
	temp.m[3][3] = 1;
	return temp;
}
Utils::mat4x4 Utils::matRotY(float yRot)
{
	mat4x4 temp;

	temp.m[0][0] = cosf(yRot);
	temp.m[0][2] = -sinf(yRot);
	temp.m[1][0] = 0;
	temp.m[1][1] = 1;
	temp.m[2][0] = sinf(yRot);
	temp.m[2][2] = cosf(yRot);
	temp.m[3][3] = 1;
	return temp;
} 
Utils::mat4x4 Utils::matRotX(float xRot)
{
	mat4x4 temp;

	temp.m[0][0] = 1;
	temp.m[1][0] = 0;
	temp.m[2][0] = 0;
	temp.m[0][1] = 0;
	temp.m[1][1] = cosf(xRot);
	temp.m[2][1] = -sinf(xRot);
	temp.m[0][2] = 0;
	temp.m[1][2] = sinf(xRot);
	temp.m[2][2] = cosf(xRot);
	temp.m[3][3] = 1;
	return temp;
}
