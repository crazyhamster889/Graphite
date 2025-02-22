#include "MathsUtils.h"
#include "Algorithms.h"
#include "Renderer.h"
#include <map>

// Creates the vertex buffer
sf::VertexArray graph(sf::Triangles);

// Checks whether the vertex is within screen limits 
bool Renderer::WithinScreenLimits(Utils::triangle triangle)
{
	for (Utils::vec3d pointCheck : triangle.p)
	{
		if (abs(pointCheck.x) > window.getSize().x && abs(pointCheck.y) > (float)window.getSize().y)
			return true;
	}
	return false;
}

// Main loop function for the renderer
void Renderer::OnUserUpdate(float width, float height)
{
	// clears the screen for next refresh
	window.clear(sf::Color::White);
	window.setView(sf::View(sf::FloatRect(0, 0, width, height)));
	graph.clear();

	// Creates the matrices required for later calculations 
	Utils::mat4x4 matRotX, matRotY, matRotZ, matRotCombined;

	// instantiates the controls object to start defining the camera position, this runs alongside the renderer (Aggregation)
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
	matRotCombined = maths.MultiplyMatrix(matRotX, matRotY);
	matRotCombined = maths.MultiplyMatrix(matRotCombined, matRotZ);

	// A list of triangles to rasterise later 
	vector<Utils::triangle> trianglesToRaster;
	
	// Loops through all the meshes, this consists of the graph itself and the grid 
	for (int i = 0; i <= (end(graphConstructor.meshes) - begin(graphConstructor.meshes)) - 1; i++)
	{
		// Loops through all the triangles, in the constructed mesh 
		for (auto tri : graphConstructor.meshes[i].tris)
		{
			// Defines the triangle data to be used at multiple 
			Utils::triangle triProjected, triTranslated, triRotatedZ;

			// Rotate the triangle using the combined rotation matrix
			for (int j = 0; j < 3; ++j) {
				maths.MultiplyMatrixVector(tri.p[j], triRotatedZ.p[j], matRotCombined);
			}

			// Apply translation (depth and X shift)
			float depthShiftFactor = 30.0f;
			float xShiftFactor = 5.0f;
			triTranslated = triRotatedZ;
			for (int j = 0; j < 3; ++j) {
				triTranslated.p[j].z += depthShiftFactor;
				triTranslated.p[j].x += xShiftFactor;
			}

			// Create the lines needed to calculate the cross product
			Utils::vec3d normal, edge1, edge2;
			edge1 = { tri.p[1].x - tri.p[0].x, tri.p[1].y - tri.p[0].y, tri.p[1].z - tri.p[0].z };
			edge2 = { tri.p[2].x - tri.p[0].x, tri.p[2].y - tri.p[0].y, tri.p[2].z - tri.p[0].z };
			// Compute normal vector using cross product
			normal = {
				edge1.y * edge2.z - edge1.z * edge2.y,
				edge1.z * edge2.x - edge1.x * edge2.z,
				edge1.x * edge2.y - edge1.y * edge2.x
			};

			// Normalise the normal vector
			float magnitude = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= magnitude; normal.y /= magnitude; normal.z /= magnitude;

			// this if statement checks if we're working on the mesh or the grid
			if (i == 0)
			{
				Utils::vec3d light_direction = { -0.5f, -0.5f, 0.0f };
				float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
				light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

				/*
				this calculates the dot product between the two vectors, the dot product is
				used to calculate the angle between the normal of the face and the light direction, 
				this is used to calculate the light intensity per face
				*/
				float dp = light_direction.x * normal.x + light_direction.y * normal.y + light_direction.z * normal.z;
				float result = std::fmax(0.0f, std::fmin(dp, 0.8f));
				triProjected.color = baseColour;
				triProjected.light = result;
			}
			else
			{
				/* 
				If the triangle is part of the grid, we set the colour to the default triangle colour and the light to 1
				This way the grid is not affected by the light source
				*/
				triProjected.color = tri.color;
				triProjected.light = 1;
			}

			// Project the triangle onto the screen by multiplying the final position by the projection matrix
			for (int j = 0; j < 3; ++j) {
				maths.MultiplyMatrixVector(triTranslated.p[j], triProjected.p[j], matProj);
				triProjected.p[j].x = (triProjected.p[j].x + 1.0f) * 0.5f * width;
				triProjected.p[j].y = (triProjected.p[j].y + 1.0f) * 0.5f * height;
			}

			// Set grid mode based on iteration index
			triProjected.grid = (i != 0);

			// add the projected triangle to the list of triangles to raster
			trianglesToRaster.push_back(triProjected);
		}
	}
	/*
	* We instantiate the algorithms class so we can sort the faces based on the average Z value
	* This is important for correct depth rendering, ensuring the faces that are closer to the camera
	* are rendered last so they appear on top of the other faces
	*/
	Algorithms algorithms;
	algorithms.ParallelMergeSort(trianglesToRaster, 0, trianglesToRaster.size() - 1);

	vector<sf::Vertex> gridVertices;

	/*
	* This code checks whether the point is within screen limits and if it's not it breaks the loop
	* then it checks whether the triangle is part of the grid or the mesh and appends to the correct list
	* it also applies the light value to each face
	*/
	for (Utils::triangle triProjected : trianglesToRaster)
	{
		for (Utils::vec3d point : triProjected.p)
		{
			if (WithinScreenLimits(triProjected))
				break;

			sf::Vertex v1(sf::Vector2f(point.x, point.y));
			v1.color = sf::Color(triProjected.color.r * triProjected.light, triProjected.color.g * triProjected.light, triProjected.color.b * triProjected.light, triProjected.color.a);
			if (!triProjected.grid)
				graph.append(v1);
			else
				gridVertices.push_back(v1);
		}	
	}
	sf::RectangleShape grid;


	/*
	* We're using the grid vertices to draw the grid lines, this is done differentially to how the graph is drawn
	* because the grid lines do not need to be filled, they need to be drawn as a line
	* so instead we do some trigonometry to find the angle between each grid point
	* we use this information along with the distance between the points to draw the line
	*/
	if (visibleGrid)
	{
		for (int i = 0; i < (end(gridVertices) - begin(gridVertices)) - 2; i += 2)
		{
			// there's 3 points in a triangle, we only want to use two of them to draw the lines
			if ((i + 1) % 3 != 0) {
				grid.setPosition(gridVertices[i].position);
				// uses arctan to calculate the angle between the two points
				float angle = atan2(gridVertices[i].position.y - gridVertices[i + 1].position.y, gridVertices[i].position.x - gridVertices[i + 1].position.x);
				grid.setRotation(angle * 180 / 3.1459f);
				// uses pythagoras to calculate the distance between the two points
				grid.setSize(sf::Vector2f(-sqrt(powf(gridVertices[i].position.y - gridVertices[i + 1].position.y, 2) + powf(gridVertices[i].position.x - gridVertices[i + 1].position.x, 2)), 0));

				grid.setOutlineColor(gridVertices[i].color);
				grid.setOutlineThickness(1);
				window.draw(grid);
			}
		}
	}

	// draws the output meshe to the screen
	window.draw(graph);
	return;
}