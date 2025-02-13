#include <string>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <Windows.h>

#include "CMathParser.h"
#include "Database.h"
#include "MathsUtils.h"
#include "Algorithms.h"
#include "Renderer.h"
#include "BuildGraph.h"
#include "CameraControls.h"

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

using namespace std;

class UserInterface 
{
public:
	UserInterface(sf::RenderWindow& targetWindow, BuildGraph& targetGraph, DatabaseClass& database);

	sf::RenderWindow& window;
	tgui::ColorPicker::Ptr colourPicker;
	BuildGraph graph;
	DatabaseClass databaseInstance;
	Renderer renderer;

	bool gridVisible;

	void ToggleGrid();
	void Render();
	void ToggleClourPicker(tgui::BackendGui& gui);
 	void Graph(string equationInput, float resolutionInput, float sliderInput, tgui::Color color);
	void loadWidgets(tgui::BackendGui& gui);
	void ReferenceItems(tgui::ListBox* ListView);
	void MainMenu(tgui::BackendGui& gui);
	void ViewSelectionMenu(tgui::BackendGui& gui);
	bool run(tgui::BackendGui& gui);
};

#endif