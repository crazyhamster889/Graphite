#include "BuildGraph.h"


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
	}
	for (float k = -gridSize; k <= gridSize; k += 0.7)
	{
		if (k == 0)
			continue;
		CreateGridLine({ k, 1, -gridSize }, { k,1,gridSize }, GridColour);
	}
	CreateGridLine({ 0, 1, 0 }, { 0, -gridSize, 0 }, sf::Color::Blue);
	CreateGridLine({ -gridSize, 1, 0 }, { gridSize, 1, 0 }, sf::Color::Red);
	CreateGridLine({ 0, 1, -gridSize }, { 0, 1, gridSize }, sf::Color::Green);

	CreateGridLine({ gridSize, 1, gridSize }, { gridSize, -gridSize, gridSize }, GridColour);
	CreateGridLine({ gridSize, 1, -gridSize }, { gridSize, -gridSize, -gridSize }, GridColour);
	CreateGridLine({ -gridSize, 1, -gridSize }, { -gridSize, -gridSize, -gridSize }, GridColour);
	CreateGridLine({ -gridSize, 1, gridSize }, { -gridSize, -gridSize, gridSize }, GridColour);

	CreateGridLine({ -gridSize, -gridSize, gridSize }, { gridSize, -gridSize, gridSize }, GridColour);
	CreateGridLine({ gridSize, -gridSize, gridSize }, { gridSize, -gridSize, -gridSize }, GridColour);
	CreateGridLine({ gridSize, -gridSize, -gridSize }, { -gridSize, -gridSize, -gridSize }, GridColour);
	CreateGridLine({ -gridSize, -gridSize, -gridSize }, { -gridSize, -gridSize, gridSize }, GridColour);
}

void BuildGraph::OnUserCreate(string equationInput, float resolutionInput)
{
	for (int i = 0; i <= (end(meshes) - begin(meshes)) - 1; i++)
	{
		meshes[i].tris.clear();
	}

	string equation = equationInput;
	database.InsertIntoDatabase(*equation.data(), *equation.data());
	parser ob;
	Algorithms algorithms;
	ob.eval_exp("z = " + to_string(resolutionInput));

	for (float x = -gridSize; x <= gridSize; x += resolutionInput)
	{
		ob.eval_exp("x = " + to_string(x));

		for (float y = -gridSize; y <= gridSize; y += resolutionInput)
		{
			ob.eval_exp("y = " + to_string(y));

			string ZXExpression = algorithms.replaceAll(equation, "y", "(Y-Z)");
			ZXExpression = algorithms.replaceAll(ZXExpression, "x", "(X-Z)");

			float zXOld = -ob.eval_exp(algorithms.replaceAll(equation, "x", "(X-Z)"));
			float zXYOld = -ob.eval_exp(ZXExpression);
			float zYOld = -ob.eval_exp(algorithms.replaceAll(equation, "y", "(Y-Z)"));
			float z = -ob.eval_exp(equation);

			if ((z < gridSize && z > -gridSize) || (zXYOld < gridSize && zXYOld > -gridSize) || (zYOld < gridSize && zYOld > -gridSize) || (zXOld < gridSize && zXOld > -gridSize)) {
				zXOld = std::fmax(-gridSize, std::fmin(zXOld, gridSize));
				zXYOld = std::fmax(-gridSize, std::fmin(zXYOld, gridSize));
				zYOld = std::fmax(-gridSize, std::fmin(zYOld, gridSize));
				z = std::fmax(-gridSize, std::fmin(z, gridSize));
				CreateQuad({ x + -resolutionInput / 2.0f, zXYOld, y - resolutionInput / 2.0f }, { x + resolutionInput / 2.0f, zYOld, y - resolutionInput / 2.0f }, { x + -resolutionInput / 2.0f, zXOld, y + resolutionInput / 2.0f }, { x + resolutionInput / 2.0f, z, y + resolutionInput / 2.0f }, 0, sf::Color::White);
			}
		}
	}

	GridBuilder();
	return;
}
