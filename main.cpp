#include <string>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <Windows.h>

#include "Parser.h"
#include "Database.h"
#include "BuildGraph.h"
#include "UserInterface.h"

using namespace std;

const float ScreenWidth = 1100;
const float ScreenHeight = 800;

sf::ContextSettings settings;
sf::RenderWindow window(sf::VideoMode(ScreenWidth, ScreenHeight), "GRAPHITE", sf::Style::Default, settings);
// Instantiates program components (Composition)
DatabaseClass database;
BuildGraph graph(database);
UserInterface UI(window, graph, database);

int main()
{
	// Sets up the window settings and hides the console window
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	settings.antialiasingLevel = 8;
	window.setVerticalSyncEnabled(true);

	// Run GUI setup
	tgui::Gui gui{ window };
	UI.run(gui);

	// Sets up the database
	database.SetupDatabase();

	while (window.isOpen())
	{
		sf::Event event;

		// Handles UI events 
		while (window.pollEvent(event))
		{
			gui.handleEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// Renders and draws the UI
		UI.Render();
		gui.draw();

		window.display();
	}
    return 0;
}
