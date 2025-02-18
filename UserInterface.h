#include <string>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <Windows.h>

#include "Parser.h"
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
	int userID;

	void ToggleGrid();
	void Render();
	void ToggleClourPicker(tgui::BackendGui& gui);
 	void Graph(string equationInput, float resolutionInput, float sliderInput, tgui::Color color);
	tgui::Button::Ptr createButton(const std::string& text, tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& gui);
	tgui::EditBox::Ptr createEditBox(const std::string& placeholder, tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group);
	tgui::CheckBox::Ptr createCheckBox(tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group);
	tgui::Slider::Ptr createSlider(tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group);
	tgui::ListBox::Ptr createListView(tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group);
	tgui::MenuBar::Ptr createMenuBar(tgui::BackendGui& gui, tgui::Group::Ptr& group);
	void populateList(tgui::ListBox::Ptr listView, DatabaseClass databaseInstance);
	void loadWidgets(tgui::BackendGui& gui);
	void ReferenceItems(tgui::ListBox* ListView);
	void MainMenu(tgui::BackendGui& gui);
	void ViewSelectionMenu(tgui::BackendGui& gui);
	bool run(tgui::BackendGui& gui);
};

#endif