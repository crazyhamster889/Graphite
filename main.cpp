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
#include "UserInterface.h"

using namespace std;

const float ScreenWidth = 1500;
const float ScreenHeight = 1000;

sf::ContextSettings settings;
sf::RenderWindow window(sf::VideoMode(ScreenWidth, ScreenHeight), "I LOVE BIRDS", sf::Style::Resize, settings);
DatabaseClass database;
BuildGraph graph(database);
UserInterface UI(window, graph, database);

int main()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	settings.antialiasingLevel = 8;
	window.setFramerateLimit(1000);
	window.setVerticalSyncEnabled(true);
	tgui::Gui gui{ window };
	UI.run(gui);
	database.SetupDatabase();
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			gui.handleEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
		}
		UI.Render();
		gui.draw();
		window.display();
	}
    return 0;
}
