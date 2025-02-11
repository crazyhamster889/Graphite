#include "UserInterface.h"
#include "Database.h"
#include "BuildGraph.h"
#include <string>
#include <functional> 
#include <Windows.h>
using namespace std;

int userID;

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

void UserInterface::Graph(string equationInput, float resolutionInput, float sliderInput, tgui::Color color)
{
	if (equationInput.empty() != 1)
	{
		graph.OnUserCreate(equationInput, resolutionInput, userID);
	}
	renderer.graphConstructor = graph;
	renderer.baseColour = color;
}

void UserInterface::loadWidgets(tgui::BackendGui& gui)
{
	auto blackTheme = tgui::Theme::create("TGUI-1.5/themes/Dark.txt");
	blackTheme->setDefault("TGUI-1.5/themes/Dark.txt");

	auto interactionPanel = tgui::Panel::create();

	interactionPanel->setSize({ "27%", "100%" });
	interactionPanel->setPosition({ "0%", "0%" });

	gui.add(interactionPanel);
	interactionPanel->setEnabled(false);

	tgui::EditBox::Ptr editBoxEquation = nullptr;
	editBoxEquation = tgui::EditBox::create();
	editBoxEquation->setSize({ "20%", "5%" });
	editBoxEquation->setPosition({ "2%", "20%" });
	editBoxEquation->setDefaultText("Equation");
	gui.add(editBoxEquation);
	string equationOutput = (string)editBoxEquation->getText();

	// LOGIN CODE



	tgui::EditBox::Ptr usernameLogin = nullptr;
	usernameLogin = tgui::EditBox::create();
	usernameLogin->setSize({ "20%", "5%" });
	usernameLogin->setPosition({ "40%", "20%" });
	usernameLogin->setDefaultText("Username");
	gui.add(usernameLogin);

	tgui::EditBox::Ptr passwordLogin = nullptr;
	passwordLogin = tgui::EditBox::create();
	passwordLogin->setSize({ "20%", "5%" });
	passwordLogin->setPosition({ "40%", "30%" });
	passwordLogin->setDefaultText("Password");
	gui.add(passwordLogin);

	//LOGIN


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

	auto createAccount = tgui::Button::create("Create Account");
	createAccount->setSize({ "20%", "5%" });
	createAccount->setPosition({ "40%", "40%" });
	createAccount->onClick([this, usernameLogin, passwordLogin, ListView]() {
		userID = databaseInstance.InsertIntoUserTable(*usernameLogin->getText().toStdString().data(), *passwordLogin->getText().toStdString().data());
		ListView->removeAllItems();

		for (string item : databaseInstance.LastEquation(userID))
		{
			ListView->addItem(item);
		}
	});
	gui.add(createAccount);

	colourPicker = tgui::ColorPicker::create();
	colourPicker->setSize({ "40%", "30%" });
	colourPicker->setPosition({ "26%", "20%" });
	colourPicker->setPositionLocked(true);
	colourPicker->setColor(tgui::Color::Cyan);
	tgui::Color color = colourPicker->getColor();
	gui.add(colourPicker);
	colourPicker->close();

	auto picture = tgui::Picture::create("Sprites/Logo.png");
	picture->setSize({ "7%", "10%" });
	picture->setPosition({"2%", "5%"});
	picture->setInheritedOpacity(0.2);
	gui.add(picture);

	ListView->onItemSelect([this,ListView, editBoxResolution, slider, color]() {
		std::string equationOutput = ListView->getSelectedItem().toStdString();
		float resolution = editBoxResolution->getText().toFloat();
		float gridSize = slider->getValue();

		Graph(equationOutput, resolution, gridSize, color);
		});


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

	auto MenuBar = tgui::MenuBar::create();

	MenuBar->setSize({ "20%", "4%" });
	MenuBar->setPosition({ "0%", "0%" });
	MenuBar->addMenu("File");
	MenuBar->addMenu("Settings");
	MenuBar->addMenu("Login");
	MenuBar->addMenu("Help");
	MenuBar->setTextSize(20);
	MenuBar->addMenuItem("File", "Save");
	MenuBar->addMenuItem("File", "Program Info");
	MenuBar->addMenuItem("File", "Exit");
	MenuBar->setWidth("5%");

	MenuBar->connectMenuItem("File", "Exit", [this, &gui]() {
		window.close();
		});
	gui.add(MenuBar);

	colourPickerButton->onClick([this,&gui]() {
		ToggleClourPicker(gui);
		});
	button->onPress([this,editBoxEquation, editBoxResolution, slider, color]() {
	std::string equationOutput = editBoxEquation->getText().toStdString();
	float resolution = editBoxResolution->getText().toFloat();
	float gridSize = slider->getValue();

	Graph(equationOutput, resolution, gridSize, color);
	});
}

void UserInterface::Render() 
{
	renderer.matProj = renderer.maths.DefineProjectionMatrix(window.getSize().y, window.getSize().x);
	renderer.OnUserUpdate();
	renderer.visibleGrid = gridVisible;
	renderer.baseColour = colourPicker->getColor();
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
