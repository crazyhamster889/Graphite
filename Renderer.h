#include <vector>
#include "MathsUtils.h"
#include "BuildGraph.h"
#include "CameraControls.h"


#ifndef RENDERER_H
#define RENDERER_H
using namespace std;

class Renderer
{
	bool WithinScreenLimits(Utils::triangle triangle);

public:

	Renderer(sf::RenderWindow& targetWindow, BuildGraph& targetGraph) : window(targetWindow), graphConstructor(targetGraph) {}

	Utils::mat4x4 matProj;
	BuildGraph graphConstructor;
	CameraControls controls;
	Utils maths;

	bool visibleGrid = false;
	tgui::Color baseColour;

	sf::RenderWindow& window;
	void OnUserUpdate(float width, float height);
};

#endif 