#include <vector>
#include "MathsUtils.h"
#include "CMathParser.h"
#include "Algorithms.h"
#include "Database.h"
#include "Renderer.h"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#ifndef BUILDGRAPH_H
#define BUILDGRAPH_H
using namespace std;

class BuildGraph
{
public:
	BuildGraph(DatabaseClass& d, Renderer& r) : renderer(r), database(d) {}

	float gridSize = 6;
	Utils::mesh meshes[2];
	DatabaseClass database;
	Renderer renderer;

	void CreateQuad(Utils::vec3d point1, Utils::vec3d point2, Utils::vec3d point3, Utils::vec3d point4, int ID, sf::Color Color);
	void CreateGridLine(Utils::vec3d Point1, Utils::vec3d Point2, sf::Color GridColour);
	void GridBuilder();
	void OnUserCreate(string equationInput, tgui::EditBox::Ptr resolutionInput);
};

#endif 