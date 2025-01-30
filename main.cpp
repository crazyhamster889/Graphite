#include <cmath>
#include <execution>
#include <string>
#include <vector>
#include <winuser.rh>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <iostream>
#include <Windows.h>

#include "CMathParser.h"
#include "Database.h"
#include "MathsUtils.h"
#include "Algorithms.h"
#include "Renderer.h"
#include "BuildGraph.h"
#include "CameraControls.h"

using namespace std;

const float ScreenWidth = 1500;
const float ScreenHeight = 1000;

sf::ContextSettings settings;
sf::RenderWindow window(sf::VideoMode(ScreenWidth, ScreenHeight), "I LOVE BIRDS", sf::Style::Resize, settings);
DatabaseClass database;
BuildGraph graph(database);
Renderer renderer(window, graph);

void updateTextSize(tgui::BackendGui& gui)
{
	// Update the text size of all widgets in the gui, based on the current window height
	const float windowHeight = gui.getView().getRect().height;
	gui.setTextSize(0.02f * windowHeight); // 7% of height
}

tgui::ColorPicker::Ptr colourPicker = nullptr;

void ToggleGrid()
{
	renderer.visibleGrid = !renderer.visibleGrid;
}
void ToggleClourPicker(tgui::BackendGui& gui)
{
	gui.add(colourPicker);
}

void TempFunction(tgui::EditBox::Ptr equationInput, tgui::EditBox::Ptr resolutionInput, tgui::Slider::Ptr sliderInput) 
{
	graph.gridSize = sliderInput->getValue();
	string equationContents = (string)equationInput->getText();
	if (equationContents.empty() != 1)
	{
		graph.OnUserCreate(equationContents, resolutionInput);
	}
	else 
	{
		graph.OnUserCreate(database.LastEquation(), resolutionInput);
	}
	renderer.graphConstructor = graph;
}

void loadWidgets(tgui::BackendGui& gui)
{
	updateTextSize(gui);

//	gui.onViewChange([&gui] { updateTextSize(gui); });
	auto blackTheme = tgui::Theme::create("TGUI-1.5/themes/Black.txt");
	blackTheme->setDefault("TGUI-1.5/themes/Black.txt");

		
	auto picture = tgui::Picture::create("Sprites/Logo.png");
	picture->setSize({ "10%", "15%" });
	picture->setInheritedOpacity(1);
	gui.add(picture);

	tgui::EditBox::Ptr editBoxEquation = nullptr;
    editBoxEquation = tgui::EditBox::create();
	editBoxEquation->setSize( { "20%", "5%" });
	editBoxEquation->setPosition({ "2%", "20%" });
	editBoxEquation->setDefaultText("Equation");
	gui.add(editBoxEquation);

	auto editBoxResolution = tgui::EditBox::copy(editBoxEquation);
	editBoxResolution->setSize({ "20%", "5%" });
	editBoxResolution->setPosition({ "2%", "30%" });
	editBoxResolution->setDefaultText("Resolution");
	gui.add(editBoxResolution);

	auto slider = tgui::Slider::create();
	slider->setSize({ "20%", "3%" });
	slider->setPosition({ "2.5%", "60%" });
	gui.add(slider);
	
    colourPicker = tgui::ColorPicker::create();
	colourPicker->setSize({ "40%", "30%" }); 
	colourPicker->setPosition({ "26%", "20%" });
	colourPicker->setPositionLocked(true);
	colourPicker->setColor(tgui::Color::Cyan);
	gui.add(colourPicker);
	colourPicker->close();
    renderer.baseColour = colourPicker->getColor();

	auto colourPickerButton = tgui::Button::create("ƒ");
	colourPickerButton->setSize({ "3%", "5%" });
	colourPickerButton->setPosition({ "22%", "20%" });
	gui.add(colourPickerButton);

	auto checkBox = tgui::CheckBox::create("Grid");
	checkBox->setSize({ "2%", "3%" });
	checkBox->setPosition({ "2%", "40%" });
	gui.add(checkBox);

	tgui::Button::Ptr button = nullptr;
    button = tgui::Button::create("Graph");
	button->setSize({ "20%", "10%" });
	button->setPosition({ "2%", "70%" });

	gui.add(button);
	checkBox->onClick(ToggleGrid);
	colourPickerButton->onClick([&gui]() {
		ToggleClourPicker(gui);
		});	
	button->onPress(&TempFunction, editBoxEquation, editBoxResolution, slider);
}

bool run(tgui::BackendGui& gui)
{
	try
	{
		loadWidgets(gui);
		return true;
	}
	catch (const tgui::Exception& e)
	{
		std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
		return false;
	}
}

int main()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	settings.antialiasingLevel = 8;
	window.setFramerateLimit(1000);
	window.setVerticalSyncEnabled(true);
	tgui::Gui gui{ window };
	run(gui);
	database.SetupDatabase();
	renderer.matProj = renderer.maths.DefineProjectionMatrix(ScreenHeight, ScreenWidth);


	while (window.isOpen())
	{

		sf::Event event;
		while (window.pollEvent(event))
		{
			gui.handleEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
		}

		renderer.OnUserUpdate();
		renderer.baseColour = colourPicker->getColor();
		cout << colourPicker->getColor().getBlue();
		gui.draw();
		window.display();
	}

    return 0;
}