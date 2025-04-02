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
		void Render();
		bool Run(tgui::BackendGui& gui);

	private:
		tgui::Theme::Ptr theme;
		bool gridVisible = false;
		bool themeEnabled = false;
		bool rendererVisible = true;
		int userID;

		sf::RenderWindow& window;
		tgui::ColorPicker::Ptr colourPicker;
		BuildGraph graph;
		DatabaseClass databaseInstance;
		Renderer renderer;

		void ToggleClourPicker(tgui::BackendGui& gui);
		void ToggleGrid();
		void ToggleLightMode();
		void HelpScreen(tgui::BackendGui& gui);
		void GraphingScreen(tgui::BackendGui& gui);
		void ReferenceItems(tgui::ListBox* ListView);
		void CreateGraph(string equationInput, string equationDescription, float resolutionInput, float sliderInput, tgui::Color color);
		void MainMenuScreen(tgui::BackendGui& gui);
		void ViewSelectionScreen(tgui::BackendGui& gui);
		void SettingsMenuScreen(tgui::BackendGui& gui);
		void populateList(tgui::ListBox::Ptr listView, DatabaseClass databaseInstance);
		tgui::Button::Ptr createButton(const std::string& text, tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& gui);
		tgui::EditBox::Ptr createEditBox(const std::string& placeholder, tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group);
		tgui::CheckBox::Ptr createCheckBox(tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group, string defaultText);
		tgui::Slider::Ptr createSlider(tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group, string defaultText);
		tgui::ListBox::Ptr createListView(tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group);
		tgui::MenuBar::Ptr createMenuBar(tgui::BackendGui& gui, tgui::Group::Ptr& group);

};

#endif