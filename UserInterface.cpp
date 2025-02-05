#include "UserInterface.h"
#include "Database.h"
#include "BuildGraph.h"
#include <string>
#include <functional> 
#include <Windows.h>
using namespace std;

UserInterface::UserInterface(sf::RenderWindow& targetWindow, BuildGraph& targetGraph, DatabaseClass& database)
	: window(targetWindow), graph(targetGraph), databaseInstance(database), renderer(window, graph){}
void UserInterface::ToggleGrid()
{
	gridVisible = !gridVisible;
}
void UserInterface::ToggleClourPicker(tgui::BackendGui& gui)
{
	gui.add(colourPicker);
}

void UserInterface::Graph(string equationInput, float resolutionInput, float sliderInput)
{
	if (equationInput.empty() != 1)
	{
		graph.OnUserCreate(equationInput, resolutionInput);
	}
	else
	{
		graph.OnUserCreate(databaseInstance.LastEquation()[0], resolutionInput);
	}
	renderer.graphConstructor = graph;
}

void UserInterface::loadWidgets(tgui::BackendGui& gui)
{
	auto blackTheme = tgui::Theme::create("TGUI-1.5/themes/Dark.txt");
	blackTheme->setDefault("TGUI-1.5/themes/Dark.txt");

	auto interactionPanel = tgui::Panel::create();

	interactionPanel->setSize({ "27%", "100%" });
	interactionPanel->setPosition({ "0%", "0%" });
	interactionPanel->setInheritedOpacity(1);

	gui.add(interactionPanel);

	tgui::EditBox::Ptr editBoxEquation = nullptr;
	editBoxEquation = tgui::EditBox::create();
	editBoxEquation->setSize({ "20%", "12%" });
	editBoxEquation->setPosition({ "2%", "15%" });
	editBoxEquation->setDefaultText("Equation");
	gui.add(editBoxEquation);
	string equationOutput = (string)editBoxEquation->getText();

	auto editBoxResolution = tgui::EditBox::copy(editBoxEquation);
	editBoxResolution->setSize({ "20%", "5%" });
	editBoxResolution->setPosition({ "2%", "30%" });
	editBoxResolution->setText("0.3");
	gui.add(editBoxResolution);
	float resolution = editBoxEquation->getText().toFloat();

	auto toolBar = tgui::Panel::create();
	toolBar->setSize({ "100%", "5%" });
	toolBar->setPosition({ "0%", "0%" });
	gui.add(toolBar);

	auto slider = tgui::Slider::create();
	slider->setSize({ "20%", "3%" });
	slider->setPosition({ "2.5%", "50%" });
	gui.add(slider);
	float gridSize = slider->getValue();

	auto ListView = tgui::ListBox::create();
	ListView->setSize({ "20%", "30%" });
	ListView->setPosition({ "2%", "55%" });
	gui.add(ListView);

	auto picture = tgui::Picture::create("Sprites/Logo.png");
	picture->setSize({ "7%", "10%" });
	picture->setPosition({"2%", "5%"});
	picture->setInheritedOpacity(0.2);
	gui.add(picture);

	for (string item : databaseInstance.LastEquation())
	{
		ListView->addItem(item);
	}
	ListView->onItemSelect([this,ListView, editBoxResolution, slider]() {
		std::string equationOutput = ListView->getSelectedItem().toStdString();
		float resolution = editBoxResolution->getText().toFloat();
		float gridSize = slider->getValue();

		Graph(equationOutput, resolution, gridSize);
		});

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
	button->setPosition({ "2%", "85%" });

	gui.add(button);
	checkBox->onClick([this]() {
		ToggleGrid();
		});

	colourPickerButton->onClick([this,&gui]() {
		ToggleClourPicker(gui);
		});
		button->onPress([this,editBoxEquation, editBoxResolution, slider]() {
		std::string equationOutput = editBoxEquation->getText().toStdString();
		float resolution = editBoxResolution->getText().toFloat();
		float gridSize = slider->getValue();

		Graph(equationOutput, resolution, gridSize);
		});
}
void UserInterface::Render() 
{
	renderer.matProj = renderer.maths.DefineProjectionMatrix(window.getSize().y, window.getSize().x);
	renderer.OnUserUpdate();
	renderer.visibleGrid = gridVisible;
}

bool UserInterface::run(tgui::BackendGui& gui) {

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
