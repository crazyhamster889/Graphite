#include "MathsUtils.h"
#include "Algorithms.h"
#include "Renderer.h"
#include <map>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

// Creates the vertex buffer
sf::VertexArray graph(sf::Triangles);

// Checks whether the vertex is within screen limits 
bool Renderer::WithinScreenLimits(Utils::vec3d& triProjected)
{
	if (abs(triProjected.x) > window.getSize().x && abs(triProjected.y) > (float)window.getSize().y)
		return true;	
	return false;
}

// Main loop function for the renderer
void Renderer::OnUserUpdate()
{
	// Creates the matrices required for later calculations 
	Utils::mat4x4 matRotX, matRotY, matRotZ, output;

	// instantiates the controls object to start defining the camera position 
	controls.DefineCameraPosition();

	/* 
	Uses the MathsUtils class to define the rotation matrices
	MatRotX: Allows the user to rotate the graph around the X axis
	MatRotY: Allows the user to rotate around the y axis
	MatRotZ: Currently isn't used, may be useful if client needs a third way of panning around the object
	*/
	matRotX = maths.matRotX(controls.xRot);
	matRotY = maths.matRotY(controls.yRot);
	matRotZ = maths.matRotZ(0);


	/* 
	* by multiplying the two rotation matrices together we get an ouput matrix that enables us to rotate on two different axises
	* We achieve this by multiplying MatRotX and MatRotY together then taking the output and multiplying that by MatRotZ
	* This is done to make sure the multiplication of the matrices is done in the right order, matrix multiplication is not commutable 
    */
	output = maths.MultiplyMatrix(matRotX, matRotY);
	output = maths.MultiplyMatrix(output, matRotZ);

	// A list of triangles to rasterise later 
	vector<Utils::triangle> trianglesToRaster;
	
	// Loops through all the meshes, this consists of the graph itself and the grid 
	for (int i = 0; i <= (end(graphConstructor.meshes) - begin(graphConstructor.meshes)) - 1; i++)
	{
		// Loops through all the triangles, in the constructed mesh 
		for (auto tri : graphConstructor.meshes[i].tris)
		{
			// Defines the triangle data to be used at multiple 
			Utils::triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			maths.MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], output);
			maths.MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], output);
			maths.MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], output);

			triTranslated = triRotatedZ;
			triTranslated.p[0].z = triRotatedZ.p[0].z + 30.0f;
			triTranslated.p[1].z = triRotatedZ.p[1].z + 30.0f;
			triTranslated.p[2].z = triRotatedZ.p[2].z + 30.0f;

			triTranslated.p[0].x = triRotatedZ.p[0].x + 3.5f;
			triTranslated.p[1].x = triRotatedZ.p[1].x + 3.5f;
			triTranslated.p[2].x = triRotatedZ.p[2].x + 3.5f;


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


			triProjected.p[0].x *= 0.5f * (float)window.getSize().x;
			triProjected.p[0].y *= 0.5f * (float)window.getSize().y;
			triProjected.p[1].x *= 0.5f * (float)window.getSize().x;
			triProjected.p[1].y *= 0.5f * (float)window.getSize().y;
			triProjected.p[2].x *= 0.5f * (float)window.getSize().x;
			triProjected.p[2].y *= 0.5f * (float)window.getSize().y;

			if (i != 0)
				triProjected.grid = true;
			else
				triProjected.grid = false;

			trianglesToRaster.push_back(triProjected);
		}
	}
	Algorithms algorithms;
	algorithms.parallelMergeSort(trianglesToRaster, 0, trianglesToRaster.size() - 1);

	window.clear(sf::Color::White);
	graph.clear();
	vector<sf::Vertex> gridVertices;

	int gridVertex = 0;

	for (Utils::triangle triProjected : trianglesToRaster)
	{
		for (Utils::vec3d point : triProjected.p)
		{
			for (Utils::vec3d pointCheck : triProjected.p)
			{
				if (WithinScreenLimits(pointCheck))
					break;
			}
			sf::Vertex v1(sf::Vector2f(point.x, point.y));
			v1.color = sf::Color(triProjected.color.r * triProjected.light, triProjected.color.g * triProjected.light, triProjected.color.b * triProjected.light, triProjected.color.a);
			if (!triProjected.grid)
				graph.append(v1);
			else
				gridVertices.push_back(v1);
		}	
		continue2:;
	}
	sf::RectangleShape grid;

	if (visibleGrid) {
		for (int i = 0; i < (end(gridVertices) - begin(gridVertices)) - 2; i += 2)
		{
			if ((i + 1) % 3 != 0) {
				grid.setPosition(gridVertices[i].position);
				float angle = atan2(gridVertices[i].position.y - gridVertices[i + 1].position.y, gridVertices[i].position.x - gridVertices[i + 1].position.x);
				grid.setRotation(angle * 180 / 3.1459f);
				grid.setSize(sf::Vector2f(-sqrt(powf(gridVertices[i].position.y - gridVertices[i + 1].position.y, 2) + powf(gridVertices[i].position.x - gridVertices[i + 1].position.x, 2)), 0));

				grid.setOutlineColor(gridVertices[i].color);
				grid.setOutlineThickness(1);
				window.draw(grid);
			}
		}
	}
	window.draw(graph);
	return;
}