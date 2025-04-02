
#include <vector>
#include "MathsUtils.h"
#include "Parser.h"
#include "Algorithms.h"
#include "Database.h"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef BUILDGRAPH_H
#define BUILDGRAPH_H
using namespace std;

class BuildGraph
{
private:
	void CreateQuad(Utils::vec3d point1, Utils::vec3d point2, Utils::vec3d point3, Utils::vec3d point4, int ID, sf::Color Color);
	void CreateGridLine(Utils::vec3d Point1, Utils::vec3d Point2, sf::Color GridColour);
	void GridBuilder();

public:
	BuildGraph(DatabaseClass& d) :  database(d) {}

	float gridSize = 6;
	Utils::mesh meshes[2];
	DatabaseClass database;
	void OnUserCreate(string equationInput, string equationDescription, float resolutionInput, int ID);
};

#endif 