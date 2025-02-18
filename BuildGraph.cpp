#include "BuildGraph.h"
#include <sstream>
#include <sqlite3.h>


void BuildGraph::CreateQuad(Utils::vec3d point1, Utils::vec3d point2, Utils::vec3d point3, Utils::vec3d point4, int ID, sf::Color Color)
{
	meshes[ID].tris.push_back({ point2,    point3,    point1, 0, false, Color });
	meshes[ID].tris.push_back({ point4,    point3,    point2, 0, false, Color });
}

void BuildGraph::CreateGridLine(Utils::vec3d Point1, Utils::vec3d Point2, sf::Color GridColour)
{
	CreateQuad(Point1, Point1, Point2, Point2, 1, GridColour);
}

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
	vector<Utils::vec3d> topCorners = {
		{gridSize, 1, gridSize },
		{gridSize, 1, -gridSize},
		{-gridSize, 1, -gridSize},
		{-gridSize, 1, gridSize}
	};

	vector<Utils::vec3d> bottomCorners;
	bottomCorners.resize(4);
	for (int i = 0; i < 4; i++) {
		bottomCorners[i] = { topCorners[i].x, -gridSize, topCorners[i].z };
		CreateGridLine(topCorners[i], bottomCorners[i], GridColour);
	}

	for (int i = 0; i < 4; i++) {
		CreateGridLine(bottomCorners[i], bottomCorners[(i + 1) % 4], GridColour);
	}
}

void BuildGraph::OnUserCreate(string equationInput, float resolutionInput, int ID)
{
	sqlite3_stmt* stmt = NULL;

	for (int i = 0; i <= (end(meshes) - begin(meshes)) - 1; i++)
		meshes[i].tris.clear();

	string equation = equationInput;

	database.InsertIntoEquationTable(*equation.data(), *equation.data(),*to_string(ID).data());
	parser ob;
	Algorithms algorithms;
	ob.EvaluateExpression("z = " + to_string(resolutionInput));

	for (float x = -gridSize; x <= gridSize; x += resolutionInput)
	{
		ob.EvaluateExpression("x = " + to_string(x));

		for (float y = -gridSize; y <= gridSize; y += resolutionInput)
		{
			ob.EvaluateExpression("y = " + to_string(y));

			std::string ZXExpression = algorithms.replaceAll(equation, "y", "(Y-Z)");
			ZXExpression = algorithms.replaceAll(ZXExpression, "x", "(X-Z)");

			vector<float> zValues = {
				-ob.EvaluateExpression(algorithms.replaceAll(equation, "x", "(X-Z)")),
				-ob.EvaluateExpression(ZXExpression),
				-ob.EvaluateExpression(algorithms.replaceAll(equation, "y", "(Y-Z)")),
				-ob.EvaluateExpression(equation)
			};

			if (any_of(zValues.begin(), zValues.end(), [&](float z) { return z > -gridSize && z < gridSize; })) 
			{
				for_each(zValues.begin(), zValues.end(), [&](float& z) { z = std::clamp(z, -gridSize, gridSize); });

				CreateQuad({ x - resolutionInput / 2.0f, zValues[1], y - resolutionInput / 2.0f },
					{ x + resolutionInput / 2.0f, zValues[2], y - resolutionInput / 2.0f },
					{ x - resolutionInput / 2.0f, zValues[0], y + resolutionInput / 2.0f },
					{ x + resolutionInput / 2.0f, zValues[3], y + resolutionInput / 2.0f }, 0, sf::Color::White);
			}
		}
	}

	GridBuilder();
	return;
}
