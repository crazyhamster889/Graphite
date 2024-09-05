
#include <cmath>
#include <execution>
#include <algorithm>
#include <string>
#include <vector>
#include <winuser.rh>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <iostream>
#include <thread>
#include <Windows.h>
#include "CMathParser.h"
#include "poolstl.hpp"
using namespace std;

const float ScreenWidth = 1500;
const float ScreenHeight = 1000;

bool visibleGrid = false;
tgui::Color baseColour;

void FillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, sf::RenderWindow& window)
{
    sf::ConvexShape convex;

    // resize it to 5 points
    convex.setPointCount(3);

    // define the points
    convex.setPoint(0, sf::Vector2f(x1, y1));
    convex.setPoint(1, sf::Vector2f(x2, y2));
    convex.setPoint(2, sf::Vector2f(x3, y3));

	window.draw(convex);

    return;
}


struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d p[3];

	float light;
	bool grid;
	sf::Color color;
};

struct  mesh
{
	vector<triangle> tris;
};

struct mat4x4
{
	float m[4][4] = { 0 };
};


mesh meshes[2];
mat4x4 matProj;

vec3d vCamera;

float fTheta = 0;

void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
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
void Task1(mat4x4& input, mat4x4& i, mat4x4& o)
{

}

void Task2(mat4x4& input, mat4x4& i, mat4x4& o)
{

}

void Task3(mat4x4& input, mat4x4& i, mat4x4& o)
{

}

void Task4(mat4x4& input, mat4x4& i, mat4x4& o)
{

}

mat4x4 MultiplyMatrix(mat4x4& i, mat4x4& o)
{
	mat4x4 input;
	input.m[0][0] = i.m[0][0] * o.m[0][0] + i.m[1][0] * o.m[0][1] + i.m[2][0] * o.m[0][2] + i.m[3][0] * o.m[0][3];
	input.m[0][1] = i.m[0][1] * o.m[0][0] + i.m[1][1] * o.m[0][1] + i.m[2][1] * o.m[0][2] + i.m[3][1] * o.m[0][3];
	input.m[0][2] = i.m[0][2] * o.m[0][0] + i.m[1][2] * o.m[0][1] + i.m[2][2] * o.m[0][2] + i.m[3][2] * o.m[0][3];
	input.m[0][3] = i.m[0][3] * o.m[0][0] + i.m[1][3] * o.m[0][1] + i.m[2][3] * o.m[0][2] + i.m[3][3] * o.m[0][3];
	// x, y
	input.m[1][0] = i.m[0][0] * o.m[1][0] + i.m[1][0] * o.m[1][1] + i.m[2][0] * o.m[1][2] + i.m[3][0] * o.m[1][3];
	input.m[1][1] = i.m[0][1] * o.m[1][0] + i.m[1][1] * o.m[1][1] + i.m[2][1] * o.m[1][2] + i.m[3][1] * o.m[1][3];
	input.m[1][2] = i.m[0][2] * o.m[1][0] + i.m[1][2] * o.m[1][1] + i.m[2][2] * o.m[1][2] + i.m[3][2] * o.m[1][3];
	input.m[1][3] = i.m[0][3] * o.m[1][0] + i.m[1][3] * o.m[1][1] + i.m[2][3] * o.m[1][2] + i.m[3][3] * o.m[1][3];

	input.m[2][0] = i.m[0][0] * o.m[2][0] + i.m[1][0] * o.m[2][1] + i.m[2][0] * o.m[2][2] + i.m[3][0] * o.m[2][3];
	input.m[2][1] = i.m[0][1] * o.m[2][0] + i.m[1][1] * o.m[2][1] + i.m[2][1] * o.m[2][2] + i.m[3][1] * o.m[2][3];
	input.m[2][2] = i.m[0][2] * o.m[2][0] + i.m[1][2] * o.m[2][1] + i.m[2][2] * o.m[2][2] + i.m[3][2] * o.m[2][3];
	input.m[2][3] = i.m[0][3] * o.m[2][0] + i.m[1][3] * o.m[2][1] + i.m[2][3] * o.m[2][2] + i.m[3][3] * o.m[2][3];

	input.m[3][0] = i.m[0][0] * o.m[3][0] + i.m[1][0] * o.m[3][1] + i.m[2][0] * o.m[3][2] + i.m[3][0] * o.m[3][3];
	input.m[3][1] = i.m[0][1] * o.m[3][0] + i.m[1][1] * o.m[3][1] + i.m[2][1] * o.m[3][2] + i.m[3][1] * o.m[3][3];
	input.m[3][2] = i.m[0][2] * o.m[3][0] + i.m[1][2] * o.m[3][1] + i.m[2][2] * o.m[3][2] + i.m[3][2] * o.m[3][3];
	input.m[3][3] = i.m[0][3] * o.m[3][0] + i.m[1][3] * o.m[3][1] + i.m[2][3] * o.m[3][2] + i.m[3][3] * o.m[3][3];

	/*std::thread t1(Task1, std::ref(input), std::ref(i), std::ref(o));
	std::thread t2(Task2, std::ref(input), std::ref(i), std::ref(o));
	std::thread t3(Task3, std::ref(input), std::ref(i), std::ref(o));
	std::thread t4(Task4, std::ref(input), std::ref(i), std::ref(o));


	t1.join();
	t2.join();
	t3.join();
	t4.join();*/


	return input;
}

string toLower(const string& s) {
	string lower = s;
	transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	return lower;
}

string replaceAll(const string& str, const string& from, const string& to) {
	if (from.empty()) return str;
	string result = str;
	string lowerResult = toLower(str);  // Convert the input string to lowercase.
	string lowerFrom = toLower(from);   // Convert the 'from' string to lowercase.

	size_t start_pos = 0;
	while ((start_pos = lowerResult.find(lowerFrom, start_pos)) != string::npos) {
		result.replace(start_pos, lowerFrom.length(), to);
		start_pos += to.length(); 
		lowerResult = toLower(result);
	}
	return result;
}

void CreateQuad(vec3d point1, vec3d point2, vec3d point3, vec3d point4, int ID, sf::Color Color)
{
	meshes[ID].tris.push_back({ point2,    point3,    point1, 0, false, Color });
	meshes[ID].tris.push_back({ point4,    point3,    point2, 0, false, Color });
}




float xRot = 0.0f;
float yRot = 0.0f;

float xInitial = 0.0f;
float yInitial = 0.0f;

float xRotInitial = 0.0f;
float yRotInitial = 0.0f;
bool InitialClick = false;

sf::VertexArray shape(sf::Triangles);

int frames = 0;

void OnUserUpdate(float fElapsedTime, sf::RenderWindow& window)
{

	mat4x4 matRotX, matRotY, matRotZ, output;
	fTheta += 0.01f;
	//POINT p;

	sf::Vector2i position = sf::Mouse::getPosition();
	//ScreenToClient(FindWindowA(NULL, "Tayler Sucks"), &p);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

		if (InitialClick == false)
		{
			yRotInitial = yRot;
			xRotInitial = xRot;
			xInitial = position.x;
			yInitial = -position.y;
			InitialClick = true;
		}
		xRot = (xRotInitial)+(yInitial + position.y) / 1000;
		yRot = (yRotInitial)+(xInitial - position.x) / 1000;
	}
	else
	{
		InitialClick = false;
	}

	matRotX.m[0][0] = 1; matRotX.m[1][0] = 0; matRotX.m[2][0] = 0;
	matRotX.m[0][1] = 0; matRotX.m[1][1] = cosf(xRot); matRotX.m[2][1] = -sinf(xRot);
	matRotX.m[0][2] = 0; matRotX.m[1][2] = sinf(xRot); matRotX.m[2][2] = cosf(xRot);
	matRotX.m[3][3] = 1;

	matRotY.m[0][0] = cosf(yRot);
	matRotY.m[0][2] = -sinf(yRot);
	matRotY.m[1][0] = 0;
	matRotY.m[1][1] = 1;
	matRotY.m[2][0] = sinf(yRot);
	matRotY.m[2][2] = cosf(yRot);
	matRotY.m[3][3] = 1;

	matRotZ.m[0][0] = cosf(0); matRotZ.m[1][0] = -sinf(0); matRotZ.m[2][0] = 0;
	matRotZ.m[0][1] = sinf(0); matRotZ.m[1][1] = cosf(0); matRotZ.m[2][1] = 0;
	matRotZ.m[0][2] = 0; matRotZ.m[1][2] = 0; matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;

	output = MultiplyMatrix(matRotX, matRotY);
	output = MultiplyMatrix(output, matRotZ);
	//	printf("%6.*lf", 3, matRotZ.m[3][0]);

	vector<triangle> vecTrianglesToRaster;


	for (int i = 0; i <= (end(meshes) - begin(meshes)) - 1; i++)
	{
		for (auto tri : meshes[i].tris)
		{

			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], output);
			MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], output);
			MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], output);

			triTranslated = triRotatedZ;
			triTranslated.p[0].z = triRotatedZ.p[0].z + 30.0f;
			triTranslated.p[1].z = triRotatedZ.p[1].z + 30.0f;
			triTranslated.p[2].z = triRotatedZ.p[2].z + 30.0f;

			triTranslated.p[0].y = triRotatedZ.p[0].y - 2.0f;
			triTranslated.p[1].y = triRotatedZ.p[1].y - 2.0f;
			triTranslated.p[2].y = triRotatedZ.p[2].y - 2.0f;

			vec3d normal, line1, line2;
			line1.x = tri.p[1].x - tri.p[0].x;
			line1.y = tri.p[1].y - tri.p[0].y;
			line1.z = tri.p[1].z - tri.p[0].z;

			line2.x = tri.p[2].x - tri.p[0].x;
			line2.y = tri.p[2].y - tri.p[0].y;
			line2.z = tri.p[2].z - tri.p[0].z;

			normal.x = line1.y * line2.z - line1.z * line2.y;
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;

			float magnitude = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= magnitude; normal.y /= magnitude; normal.z /= magnitude;



			/*if (normal.x * (triTranslated.p[0].x - vCamera.x) +
				normal.y * (triTranslated.p[0].y - vCamera.y) +
				normal.z * (triTranslated.p[0].z - vCamera.z) < 0)
			{*/

			if (i == 0) 
			{
				vec3d light_direction = { -0.5f, -0.5f, 0.0f };
				float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
				light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

				float dp = light_direction.x * normal.x + light_direction.y * normal.y + light_direction.z * normal.z;
				float result = std::fmax(0.0f, std::fmin(dp /* *(0.6f + cosf(fTheta))*/, 0.8f));
				triProjected.color = baseColour;


				triProjected.light = result;
			}
			else
			{
				triProjected.color = tri.color;
				triProjected.light = 1;
			}


			MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
			MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
			MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

			triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
			triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
			triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;


			triProjected.p[0].x *= 0.5f * (float)ScreenWidth;
			triProjected.p[0].y *= 0.5f * (float)ScreenHeight;
			triProjected.p[1].x *= 0.5f * (float)ScreenWidth;
			triProjected.p[1].y *= 0.5f * (float)ScreenHeight;
			triProjected.p[2].x *= 0.5f * (float)ScreenWidth;
			triProjected.p[2].y *= 0.5f * (float)ScreenHeight;

			if (i != 0) 
				triProjected.grid = true;
			else
				triProjected.grid = false;

			vecTrianglesToRaster.push_back(triProjected);
		}
	}
	
	sort(std::execution::par, vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
		{
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
			return z1 > z2;
		});
	window.clear(sf::Color::White);
	shape.clear();
	vector<sf::Vertex> gridVertices;

	int gridVertex = 0;

	for (auto& triProjected : vecTrianglesToRaster)
	{
	//	if (abs(triProjected.p[0].x) < ScreenWidth && abs(triProjected.p[0].y) < ScreenHeight || triProjected.grid) {
		//	if (abs(triProjected.p[1].x) < ScreenWidth && abs(triProjected.p[1].y) < ScreenHeight || triProjected.grid) {
			//	if (abs(triProjected.p[2].x) < ScreenWidth && abs(triProjected.p[2].y) < ScreenHeight || triProjected.grid) {

					sf::Vertex v1(sf::Vector2f(triProjected.p[0].x, triProjected.p[0].y));
					sf::Vertex v2(sf::Vector2f(triProjected.p[1].x, triProjected.p[1].y));
					sf::Vertex v3(sf::Vector2f(triProjected.p[2].x, triProjected.p[2].y));
					v2.color = sf::Color(triProjected.color.r * triProjected.light, triProjected.color.g * triProjected.light, triProjected.color.b * triProjected.light, triProjected.color.a);
					v1.color = sf::Color(triProjected.color.r * triProjected.light, triProjected.color.g * triProjected.light, triProjected.color.b * triProjected.light, triProjected.color.a);
					v3.color = sf::Color(triProjected.color.r * triProjected.light, triProjected.color.g * triProjected.light, triProjected.color.b * triProjected.light, triProjected.color.a);
					if (!triProjected.grid) 
					{
						shape.append(v1);
						shape.append(v2);
						shape.append(v3);
					}
					else
					{
						gridVertices.push_back(v1);
						gridVertices.push_back(v2);

						gridVertices.push_back(v2);
						gridVertices.push_back(v3);

						gridVertices.push_back(v3);
						gridVertices.push_back(v1);
					}
				//	sf::RectangleShape line(sf::Vector2f(150, 5));
					//sf::RectangleShape line(sf::Vector2f(150, 5));
			//	}
		//	}
	//	}
		//}
		/*	DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y,
				PIXEL_QUARTER, FG_BLACK);*/

	}
	sf::RectangleShape grid;

	if (visibleGrid) {
		for (int i = 0; i < (end(gridVertices) - begin(gridVertices)) - 2; i += 2)
		{
			grid.setPosition(gridVertices[i].position);
			float angle = atan2(gridVertices[i].position.y - gridVertices[i + 1].position.y, gridVertices[i].position.x - gridVertices[i + 1].position.x);
			grid.setRotation(angle * 180 / PI);
			grid.setSize(sf::Vector2f(-sqrt(powf(gridVertices[i].position.y - gridVertices[i + 1].position.y, 2) + powf(gridVertices[i].position.x - gridVertices[i + 1].position.x, 2)), 0));

			grid.setOutlineColor(gridVertices[i].color);
			grid.setOutlineThickness(1);
			window.draw(grid);
		}
	}
	window.draw(shape);

	if (visibleGrid) {
		for (int i = 0; i < (end(gridVertices) - begin(gridVertices)) - 2; i += 2)
		{
			grid.setPosition(gridVertices[i].position);
			float angle = atan2(gridVertices[i].position.y - gridVertices[i + 1].position.y, gridVertices[i].position.x - gridVertices[i + 1].position.x);
			grid.setRotation(angle * 180 / PI);
			grid.setSize(sf::Vector2f(-sqrt(powf(gridVertices[i].position.y - gridVertices[i + 1].position.y, 2) + powf(gridVertices[i].position.x - gridVertices[i + 1].position.x, 2)), 0));

			grid.setOutlineColor({ gridVertices[i].color.r, gridVertices[i].color.g, gridVertices[i].color.b, 10 });
			grid.setOutlineThickness(1);
			window.draw(grid);
		}
	}
	frames++;
	return;
}

void OnUserCreate(tgui::EditBox::Ptr equationInput, tgui::EditBox::Ptr resolutionInput)
{
	for (int i = 0; i <= (end(meshes) - begin(meshes)) - 1; i++)
	{
		meshes[i].tris.clear();
	}

	float size = resolutionInput->getText().toFloat();

	string equation = (string)equationInput->getText();
	parser ob;

	ob.eval_exp("z = " + to_string(size));

	for (float x = -6.0f; x <= 6.0f; x += size)
	{
		ob.eval_exp("x = " + to_string(x));

		for (float y = -6.0f; y <= 6.0f; y += size)
		{
			ob.eval_exp("y = " + to_string(y));

			string ZXExpression = replaceAll(equation, "y", "(Y-Z)");
			ZXExpression = replaceAll(ZXExpression, "x", "(X-Z)");

			float zXOld = -ob.eval_exp(replaceAll(equation, "x", "(X-Z)"));
			float zXYOld = -ob.eval_exp(ZXExpression);
			float zYOld = -ob.eval_exp(replaceAll(equation, "y", "(Y-Z)"));
			float z = -ob.eval_exp(equation);

			if ((z < 7 && z > -7) || (zXYOld < 7 && zXYOld > -7) || (zYOld < 7 && zYOld > -7) || (zXOld < 7 && zXOld > -7)) {
				zXOld = std::fmax(-6, std::fmin(zXOld, 6));
				zXYOld = std::fmax(-6, std::fmin(zXYOld, 6));
				zYOld = std::fmax(-6, std::fmin(zYOld, 6));
				z = std::fmax(-6, std::fmin(z, 6));
				CreateQuad({ x + -size / 2.0f, zXYOld, y - size / 2.0f }, { x + size / 2.0f, zYOld, y - size / 2.0f }, { x + -size / 2.0f, zXOld, y + size / 2.0f }, { x + size / 2.0f, z, y + size / 2.0f }, 0, baseColour);
			}
		}
	}

	sf::Color GridColour = { 150,150,150,60 };

	for (float j = -6.0f; j <= 6.0f; j += 1)
	{
		if (j == 0)
			continue;
		CreateQuad({ -6, 1, j }, { -6, 1, j }, { 6, 1, j }, { 6, 1, j }, 1, GridColour);
	}
	for (float k = -6.0f; k <= 6.0f; k += 1)
	{
		if (k == 0)
			continue;
		CreateQuad({ k, 1, -6 }, { k, 1, -6 }, { k,1,6 }, { k, 1, 6 }, 1, GridColour);
	}
	CreateQuad({ 0, 1, 0 }, { 0, 1, 0 }, { 0, -6, 0 }, { 0, -6, 0 }, 1, sf::Color::Blue);
	CreateQuad({ -6, 1, 0 }, { 6, 1, 0 }, { 6, 1, 0 }, { 6, 1, 0 }, 1, sf::Color::Red);
	CreateQuad({ 0, 1, -6 }, { 0, 1, -6 }, { 0, 1, 6 }, { 0, 1, 6 }, 1, sf::Color::Green);

	CreateQuad({ 6, 1, 6 }, { 6, 1, 6 }, { 6, -6, 6 }, { 6, -6, 6 }, 1, GridColour);
	CreateQuad({ 6, 1, -6 }, { 6, 1, -6 }, { 6, -6, -6 }, { 6, -6, -6 }, 1, GridColour);
	CreateQuad({ -6, 1, -6 }, { -6, 1, -6 }, { -6, -6, -6 }, { -6, -6, -6 }, 1, GridColour);
	CreateQuad({ -6, 1, 6 }, { -6, 1, 6 }, { -6, -6, 6 }, { -6, -6, 6 }, 1, GridColour);

	CreateQuad({ -6, -6, 6 }, { -6, -6, 6 }, { 6, -6, 6 }, { 6, -6, 6 }, 1, GridColour);
	CreateQuad({ 6, -6, 6 }, { 6, -6, 6 }, { 6, -6, -6 }, { 6, -6, -6 }, 1, GridColour);
	CreateQuad({ 6, -6, -6 }, { 6, -6, -6 }, { -6, -6, -6 }, { -6, -6, -6 }, 1, GridColour);
	CreateQuad({ -6, -6, -6 }, { -6, -6, -6 }, { -6, -6, 6 }, { -6, -6, 6 }, 1, GridColour);

	/*CreateQuad({6, 0, 6}, {6, 0, 6}, {6,0,-6}, {6, 0, -6}, 1);
	CreateQuad({ 6, 0, -6 }, { 6, 0, -6 }, { -6,0,-6 }, { -6, 0, -6 }, 1);
	CreateQuad({ -6, 0, -6 }, { -6, 0, -6 }, { -6,0,6 }, { -6, 0, 6 }, 1);*/

	float nearPlane = 10.0f;
	float farPlane = 100.0f;
	float fov = 40.0f;
	float aspectRatio = (float)ScreenHeight / (float)ScreenWidth;
	float fovRad = 1.0f / tanf(fov * 0.5f / 180 * 3.14159f);

	matProj.m[0][0] = aspectRatio * fovRad;
	matProj.m[1][1] = fovRad;
	matProj.m[2][2] = farPlane / (farPlane - nearPlane);
	matProj.m[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;

	return;
}

void login(tgui::EditBox::Ptr username, tgui::EditBox::Ptr password)
{
	std::cout << "Username: " << username->getText() << std::endl;
	std::cout << "Password: " << password->getText() << std::endl;
}


void updateTextSize(tgui::BackendGui& gui)
{
	// Update the text size of all widgets in the gui, based on the current window height
	const float windowHeight = gui.getView().getRect().height;
	gui.setTextSize(0.02f * windowHeight); // 7% of height
}

void ToggleGrid() 
{
	visibleGrid = !visibleGrid;
}

tgui::ColorPicker::Ptr colourPicker;

void loadWidgets(tgui::BackendGui& gui)
{
	updateTextSize(gui);

//	gui.onViewChange([&gui] { updateTextSize(gui); });
	auto blackTheme = tgui::Theme::create("TGUI-1.5/themes/Black.txt");
	blackTheme->setDefault("TGUI-1.5/themes/Black.txt");

		
	auto picture = tgui::Picture::create("Logo.png");
	picture->setSize({ "10%", "15%" });
	picture->setInheritedOpacity(1);
	gui.add(picture);

	tgui::EditBox::Ptr editBoxEquation = nullptr;
    editBoxEquation = tgui::EditBox::create();
	editBoxEquation->setSize( { "20%", "5%" });
	editBoxEquation->setPosition({ "2%", "20%" });
	editBoxEquation->setDefaultText("Equation");
	gui.add(editBoxEquation);

	auto editBoxResolution = tgui::EditBox::copy(editBoxEquation);
	editBoxResolution->setSize({ "20%", "5%" });
	editBoxResolution->setPosition({ "2%", "30%" });
	editBoxResolution->setDefaultText("Resolution");
	gui.add(editBoxResolution);
	
    colourPicker = tgui::ColorPicker::create();
	colourPicker->setSize({ "40%", "40%" }); 
	colourPicker->setPosition({ "25%", "20%" });
	gui.add(colourPicker);
    baseColour = colourPicker->getColor();

	auto checkBox = tgui::CheckBox::create("Grid");
	checkBox->setSize({ "2%", "3%" });
	checkBox->setPosition({ "2%", "40%" });
	gui.add(checkBox);

	tgui::Button::Ptr button = nullptr;
    button = tgui::Button::create("Graph");
	button->setSize({ "20%", "10%" });
	button->setPosition({ "2%", "70%" });


	gui.add(button);
	checkBox->onClick(ToggleGrid);
	button->onPress(&OnUserCreate, editBoxEquation, editBoxResolution);
}

bool run(tgui::BackendGui& gui)
{
	try
	{
		loadWidgets(gui);
		return true;
	}
	catch (const tgui::Exception& e)
	{
		std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
		return false;
	}
}

int main()
{
	sf::Clock clock;

	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(ScreenWidth, ScreenHeight), "I LOVE BIRDS", sf::Style::Resize, settings);
	window.setFramerateLimit(1000);
	window.setVerticalSyncEnabled(true);
	tgui::Gui gui{ window };
	run(gui);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			gui.handleEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
		}

		OnUserUpdate(0.1f, window);
		baseColour = colourPicker->getColor();
		cout << colourPicker->getColor().getBlue();
		gui.draw();
		//cout << clock.getElapsedTime().asSeconds();
		// start window loop, do the event loop, and clear and draw (the map)
		if (clock.getElapsedTime().asSeconds() >= 1.f)
		{
			clock.restart();
			cout << std::to_string(frames);
			frames = 0;
		}

		window.display();

	}

    return 0;
}

