#include <vector>
#include "MathsUtils.h"
#include "BuildGraph.h"
#include "CameraControls.h"


#ifndef RENDERER_H
#define RENDERER_H
using namespace std;

class Renderer
{
	public:
		void OnUserUpdate(float width, float height);
		Renderer(sf::RenderWindow& targetWindow, BuildGraph& targetGraph) : window(targetWindow), graphConstructor(targetGraph) {}
		BuildGraph graphConstructor;
		CameraControls controls;
		Utils maths;
		Utils::mat4x4 matProj;
		tgui::Color baseColour;
		bool visibleGrid = false;

	private:
		bool WithinScreenLimits(Utils::triangle triangle);
		sf::RenderWindow& window;
};

#endif 