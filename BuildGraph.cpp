#include "BuildGraph.h"
#include <sstream>
#include <sqlite3.h>

// Create a quad with the given points and color
void BuildGraph::CreateQuad(Utils::vec3d point1, Utils::vec3d point2, Utils::vec3d point3, Utils::vec3d point4, int ID, sf::Color Color)
{
	meshes[ID].tris.push_back({ point2,    point3,    point1, 0, false, Color });
	meshes[ID].tris.push_back({ point4,    point3,    point2, 0, false, Color });
}

// creates a grid line by abusing the CreateQuad function
void BuildGraph::CreateGridLine(Utils::vec3d Point1, Utils::vec3d Point2, sf::Color GridColour)
{
	CreateQuad(Point1, Point1, Point2, Point2, 1, GridColour);
}

// Create the grid
void BuildGraph::GridBuilder()
{
	sf::Color GridColour = { 150,150,150,60 };

	for (float j = -gridSize; j <= gridSize; j += 0.7)
	{
		if (j == 0)
			continue;
		CreateGridLine({ -gridSize, 1, j }, { gridSize, 1, j }, GridColour);
		CreateGridLine({ j, 1, -gridSize }, { j,1,gridSize }, GridColour);
	}
	// Define axis grid lines with colors
	CreateGridLine({ 0, 1, 0 }, { 0, -gridSize, 0 }, sf::Color::Blue);
	CreateGridLine({ -gridSize, 1, 0 }, { gridSize, 1, 0 }, sf::Color::Red);
	CreateGridLine({ 0, 1, -gridSize }, { 0, 1, gridSize }, sf::Color::Green);

	// Define grid corners
	vector<Utils::vec3d> bottomCorners = {
		{gridSize, 1, gridSize },
		{gridSize, 1, -gridSize},
		{-gridSize, 1, -gridSize},
		{-gridSize, 1, gridSize}
	};

	vector<Utils::vec3d> topCorners;
	topCorners.resize(4);
	// defines the bottom corners based on the top corners, also creates the vertical grid lines 
	for (int i = 0; i < 4; i++) {
		topCorners[i] = { bottomCorners[i].x, -gridSize, bottomCorners[i].z };
		CreateGridLine(bottomCorners[i], topCorners[i], GridColour);
	}
	// creates the horizontal grid lines
	for (int i = 0; i < 4; i++) {
		CreateGridLine(topCorners[i], topCorners[(i + 1) % 4], GridColour);
	}
}

void BuildGraph::OnUserCreate(string equationInput, string equationDescription, float resolutionInput, int ID)
{
	sqlite3_stmt* stmt = NULL;
	if (resolutionInput < 0.1f) 
		MessageBox(NULL, L"Resolution too low, defaulted to 0.1", L"Error", MB_OK);
	
	resolutionInput = max(0.1f, resolutionInput);
	bool isGraphBounded = false;

	for (int i = 0; i <= (end(meshes) - begin(meshes)) - 1; i++)
		meshes[i].tris.clear();

	string equation = equationInput;
	database.InsertIntoEquationTable(*equationDescription.data(), *equation.data(), *to_string(ID).c_str());
	// instantiate the equation parser and algorithms class (Composition)
	parser ob;
	Algorithms algorithms;
	// Defines the Z variable for the equation parser, this is just used to store the resolution value 
	ob.EvaluateExpression("z = " + to_string(resolutionInput));

	// loops through the x coordinates based on the resolution of the graph
	for (float x = -gridSize; x <= gridSize; x += resolutionInput)
	{
		// Defines the X variable for the equation parser 
		ob.EvaluateExpression("x = " + to_string(x));

		for (float y = -gridSize; y <= gridSize; y += resolutionInput)
		{
			// Defines the Y variable for the equation parser 
			ob.EvaluateExpression("y = " + to_string(y));

			// each face in the graph is defined by 4 points, which is made up of two triangles
			// usually we would work triangles but when building the graph it's a lot easier to work with quads
			string ZXExpression = algorithms.replaceAll(equation, "y", "(Y-Z)");
			ZXExpression = algorithms.replaceAll(ZXExpression, "x", "(X-Z)");

			// defines the 4 Z values needed to create the quad, some of the points need to link up to the previous quads
			// so we need to evaluate the equation at the current point and the previous point
			vector<float> zValues = {
				-ob.EvaluateExpression(algorithms.replaceAll(equation, "x", "(X-Z)")),
				-ob.EvaluateExpression(ZXExpression),
				-ob.EvaluateExpression(algorithms.replaceAll(equation, "y", "(Y-Z)")),
				-ob.EvaluateExpression(equation)
			};
			// if any of the grid vertices are within the grid volume then we allow the face to be rendered 
			if (any_of(zValues.begin(), zValues.end(), [&](float z) { return z > -gridSize && z < gridSize; })) 
			{
				isGraphBounded = true;
				// but we becausee we're allowing the face to render if at least one of the vertices are within the volume
				// we need to clamp the values to the grid volume
				for_each(zValues.begin(), zValues.end(), [&](float& z) { z = std::clamp(z, -gridSize, gridSize); });

				// render the quad with parser defined values
				CreateQuad({ x - resolutionInput / 2.0f, zValues[1], y - resolutionInput / 2.0f },
					{ x + resolutionInput / 2.0f, zValues[2], y - resolutionInput / 2.0f },
					{ x - resolutionInput / 2.0f, zValues[0], y + resolutionInput / 2.0f },
					{ x + resolutionInput / 2.0f, zValues[3], y + resolutionInput / 2.0f }, 0, sf::Color::White);
			}
		}
	}

	if (isGraphBounded == false) {
		MessageBox(NULL, L"Graph is not bounded", L"Error", MB_OK);
	}

	// create the grid
	GridBuilder();
	return;
}
