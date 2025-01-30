#include "MathsUtils.h"
#include "Algorithms.h"
#include "Renderer.h"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

sf::VertexArray graph(sf::Triangles);

void Renderer::FillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, sf::RenderWindow& window)
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

void Renderer::OnUserUpdate()
{
	Utils::mat4x4 matRotX, matRotY, matRotZ, output;
	controls.DefineCameraPosition();

	matRotX = maths.matRotX(controls.xRot);
	matRotY = maths.matRotY(controls.yRot);
	matRotZ = maths.matRotZ(0);

	output = maths.MultiplyMatrix(matRotX, matRotY);
	output = maths.MultiplyMatrix(output, matRotZ);

	vector<Utils::triangle> vecTrianglesToRaster;
	

	for (int i = 0; i <= (end(meshes) - begin(meshes)) - 1; i++)
	{
		for (auto tri : meshes[i].tris)
		{
			Utils::triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			maths.MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], output);
			maths.MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], output);
			maths.MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], output);

			triTranslated = triRotatedZ;
			triTranslated.p[0].z = triRotatedZ.p[0].z + 30.0f;
			triTranslated.p[1].z = triRotatedZ.p[1].z + 30.0f;
			triTranslated.p[2].z = triRotatedZ.p[2].z + 30.0f;

			Utils::vec3d normal, line1, line2;
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

			if (i == 0)
			{
				Utils::vec3d light_direction = { -0.5f, -0.5f, 0.0f };
				float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
				light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

				float dp = light_direction.x * normal.x + light_direction.y * normal.y + light_direction.z * normal.z;
				float result = std::fmax(0.0f, std::fmin(dp, 0.8f));
				triProjected.color = baseColour;


				triProjected.light = result;
			}
			else
			{
				triProjected.color = tri.color;
				triProjected.light = 1;
			}


			maths.MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
			maths.MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
			maths.MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

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
	Algorithms algorithms;
	algorithms.parallelMergeSort(vecTrianglesToRaster, 0, vecTrianglesToRaster.size() - 1);

	window.clear(sf::Color::White);
	graph.clear();
	vector<sf::Vertex> gridVertices;

	int gridVertex = 0;

	for (auto& triProjected : vecTrianglesToRaster)
	{
		if (abs(triProjected.p[0].x) < ScreenWidth && abs(triProjected.p[0].y) < ScreenHeight || triProjected.grid) {
			if (abs(triProjected.p[1].x) < ScreenWidth && abs(triProjected.p[1].y) < ScreenHeight || triProjected.grid) {
				if (abs(triProjected.p[2].x) < ScreenWidth && abs(triProjected.p[2].y) < ScreenHeight || triProjected.grid) {

					sf::Vertex v1(sf::Vector2f(triProjected.p[0].x, triProjected.p[0].y));
					sf::Vertex v2(sf::Vector2f(triProjected.p[1].x, triProjected.p[1].y));
					sf::Vertex v3(sf::Vector2f(triProjected.p[2].x, triProjected.p[2].y));
					v2.color = sf::Color(triProjected.color.r * triProjected.light, triProjected.color.g * triProjected.light, triProjected.color.b * triProjected.light, triProjected.color.a);
					v1.color = sf::Color(triProjected.color.r * triProjected.light, triProjected.color.g * triProjected.light, triProjected.color.b * triProjected.light, triProjected.color.a);
					v3.color = sf::Color(triProjected.color.r * triProjected.light, triProjected.color.g * triProjected.light, triProjected.color.b * triProjected.light, triProjected.color.a);
					if (!triProjected.grid)
					{
						graph.append(v1);
						graph.append(v2);
						graph.append(v3);
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
				}
			}
		}

	}
	sf::RectangleShape grid;

	if (visibleGrid) {
		for (int i = 0; i < (end(gridVertices) - begin(gridVertices)) - 2; i += 2)
		{
			grid.setPosition(gridVertices[i].position);
			float angle = atan2(gridVertices[i].position.y - gridVertices[i + 1].position.y, gridVertices[i].position.x - gridVertices[i + 1].position.x);
			grid.setRotation(angle * 180 / 3.1459f);
			grid.setSize(sf::Vector2f(-sqrt(powf(gridVertices[i].position.y - gridVertices[i + 1].position.y, 2) + powf(gridVertices[i].position.x - gridVertices[i + 1].position.x, 2)), 0));

			grid.setOutlineColor(gridVertices[i].color);
			grid.setOutlineThickness(1);
			window.draw(grid);
		}
	}
	window.draw(graph);

	if (visibleGrid) {
		for (int i = 0; i < (end(gridVertices) - begin(gridVertices)) - 2; i += 2)
		{
			grid.setPosition(gridVertices[i].position);
			float angle = atan2(gridVertices[i].position.y - gridVertices[i + 1].position.y, gridVertices[i].position.x - gridVertices[i + 1].position.x);
			grid.setRotation(angle * 180 / 3.1459f);
			grid.setSize(sf::Vector2f(-sqrt(powf(gridVertices[i].position.y - gridVertices[i + 1].position.y, 2) + powf(gridVertices[i].position.x - gridVertices[i + 1].position.x, 2)), 0));

			grid.setOutlineColor({ gridVertices[i].color.r, gridVertices[i].color.g, gridVertices[i].color.b, 10 });
			grid.setOutlineThickness(1);
			window.draw(grid);
		}
	}
	return;
}