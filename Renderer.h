#include <vector>
#include "MathsUtils.h"
#include "BuildGraph.h"
#include "CameraControls.h"


#ifndef RENDERER_H
#define RENDERER_H
using namespace std;

class Renderer
{
	bool WithinScreenLimits(Utils::vec3d& triProjected);

public:

	Renderer(sf::RenderWindow& targetWindow, BuildGraph& targetGraph) : window(targetWindow), graphConstructor(targetGraph) {}

	Utils::mat4x4 matProj;
	BuildGraph graphConstructor;
	CameraControls controls;
	Utils maths;

	const float ScreenWidth = 1500;
	const float ScreenHeight = 1000;

	bool visibleGrid = false;
	tgui::Color baseColour;

	sf::RenderWindow& window;
	void FillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, sf::RenderWindow& window);
	void OnUserUpdate();
};

#endif 