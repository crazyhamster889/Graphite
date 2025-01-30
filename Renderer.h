#include <vector>
#include "MathsUtils.h"
#include "CameraControls.h"


#ifndef RENDERER_H
#define RENDERER_H
using namespace std;

class Renderer
{

public:

	Renderer(sf::RenderWindow& targetWindow) : window(targetWindow) {}

	Utils::mesh meshes[2];
	Utils::mat4x4 matProj;
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