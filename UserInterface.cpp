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

void UserInterface::MainMenu(tgui::BackendGui& gui)
{
	tgui::Button::Ptr createNewProject = nullptr;
	createNewProject = tgui::Button::create("Create Project");
	createNewProject->setSize({ "30%", "10%" });
	createNewProject->setPosition({ "35%", "35%" });

	gui.add(createNewProject);

	tgui::Button::Ptr settings = nullptr;
	settings = tgui::Button::create("Settings");
	settings->setSize({ "30%", "10%" });
	settings->setPosition({ "35%", "50%" });

	gui.add(settings);

	tgui::Button::Ptr help = nullptr;
	help = tgui::Button::create("Help");
	help->setSize({ "30%", "10%" });
	help->setPosition({ "35%", "60%" });

	gui.add(help);

	tgui::Button::Ptr selectGraphs = nullptr;
	selectGraphs = tgui::Button::create("Select Graphs");
	selectGraphs->setSize({ "30%", "10%" });
	selectGraphs->setPosition({ "35%", "70%" });

	gui.add(selectGraphs);

	createNewProject->onClick([this, &gui]() {
		gui.removeAllWidgets();
		loadWidgets(gui);
		});

	selectGraphs->onClick([this, &gui]() {
		gui.removeAllWidgets();
		ViewSelectionMenu(gui);
		});

}

void UserInterface::ViewSelectionMenu(tgui::BackendGui& gui)
{

	tgui::EditBox::Ptr usernameLogin = nullptr;
	usernameLogin = tgui::EditBox::create();
	usernameLogin->setSize({ "35%", "5%" });
	usernameLogin->setPosition({ "2%", "20%" });
	usernameLogin->setDefaultText("Username");
	gui.add(usernameLogin);

	tgui::EditBox::Ptr passwordLogin = nullptr;
	passwordLogin = tgui::EditBox::create();
	passwordLogin->setSize({ "35%", "5%" });
	passwordLogin->setPosition({ "2%", "30%" });
	passwordLogin->setDefaultText("Password");
	gui.add(passwordLogin);

	tgui::EditBox::Ptr classLogin = nullptr;
	classLogin = tgui::EditBox::create();
	classLogin->setSize({ "15%", "5%" });
	classLogin->setPosition({ "2%", "35%" });
	classLogin->setDefaultText("Class");
	gui.add(classLogin);

	tgui::EditBox::Ptr courseName = nullptr;
	courseName = tgui::EditBox::create();
	courseName->setSize({ "15%", "5%" });
	courseName->setPosition({ "22%", "35%" });
	courseName->setDefaultText("Course");
	gui.add(courseName);

	tgui::EditBox::Ptr courseSubject = nullptr;
	courseSubject = tgui::EditBox::create();
	courseSubject->setSize({ "15%", "5%" });
	courseSubject->setPosition({ "22%", "40%" });
	courseSubject->setDefaultText("Course Subject");
	gui.add(courseSubject);

	auto ListView = tgui::ListBox::create();
	ListView->setSize({ "35%", "25%" });
	ListView->setPosition({ "2%", "65%" });
	gui.add(ListView);

	auto createCourse = tgui::Button::create("Create Course");
	createCourse->setSize({ "15%", "10%" });
	createCourse->setPosition({ "22%", "45%" });
	createCourse->onClick([this, courseSubject, courseName]() {
		databaseInstance.InsertIntoCourseTable(*courseName->getText().toStdString().data(), *courseSubject->getText().toStdString().data());
		});

	gui.add(createCourse);

	auto createClass = tgui::Button::create("Create Class");
	createClass->setSize({ "15%", "10%" });
	createClass->setPosition({ "2%", "45%" });
	createClass->onClick([this, classLogin, courseName]() {
		cout << "Creating Class";
		databaseInstance.InsertIntoClassTable(*classLogin->getText().toStdString().data(), *courseName->getText().toStdString().data());
		});


	gui.add(createClass);

	auto createAccount = tgui::Button::create("Create Account / Login");
	createAccount->setSize({ "35%", "10%" });
	createAccount->setPosition({ "2%", "55%" });
	createAccount->onClick([this, usernameLogin, passwordLogin, ListView, courseName, classLogin]() {
		userID = databaseInstance.InsertIntoUserTable(*usernameLogin->getText().toStdString().data(), 
													  *passwordLogin->getText().toStdString().data(),
													  *classLogin->getText().toStdString().data());
		ListView->removeAllItems();

		for (string item : databaseInstance.LastEquation(userID))
		{
			ListView->addItem(item);
		}
		});
	gui.add(createAccount);


	auto Graphing = tgui::Button::create("Graphing page");
	Graphing->setSize({ "35%", "10%" });
	Graphing->setPosition({ "2%", "80%" });
	Graphing->onClick([this, &gui, ListView]() {
		gui.removeAllWidgets();
		loadWidgets(gui);
		});
	gui.add(Graphing);
}

void UserInterface::loadWidgets(tgui::BackendGui& gui)
{
	auto interactionPanel = tgui::Panel::create();

	interactionPanel->setSize({ "27%", "100%" });
	interactionPanel->setPosition({ "0%", "0%" });

	gui.add(interactionPanel);

	tgui::EditBox::Ptr editBoxEquation = nullptr;
	editBoxEquation = tgui::EditBox::create();
	editBoxEquation->setSize({ "20%", "5%" });
	editBoxEquation->setPosition({ "2%", "20%" });
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

	auto ListView = tgui::ListBox::create();
	ListView->setSize({ "20%", "25%" });
	ListView->setPosition({ "2%", "60%" });
	gui.add(ListView);

	ListView->onItemSelect([this, ListView, editBoxResolution, slider, color]() {
		std::string equationOutput = ListView->getSelectedItem().toStdString();
		float resolution = editBoxResolution->getText().toFloat();
		float gridSize = slider->getValue();

		Graph(equationOutput, resolution, gridSize, color);
		});

	for (string item : databaseInstance.LastEquation(userID))
	{
		ListView->addItem(item);
	}

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

	MenuBar->setSize({ "20%", "3%" });
	MenuBar->setPosition({ "0%", "0%" });
	MenuBar->addMenu("File");
	MenuBar->addMenu("Settings");
	MenuBar->addMenuItem("Settings Page");
	MenuBar->addMenu("Login");
	MenuBar->addMenuItem("Account Page");
	MenuBar->addMenu("Help");
	MenuBar->setTextSize(20);
	MenuBar->addMenuItem("File", "Save");
	MenuBar->addMenuItem("File", "Program Info");
	MenuBar->addMenuItem("File", "Exit");

	gui.add(MenuBar);

	MenuBar->connectMenuItem("File", "Exit", [this, &gui]() {
		window.close();
		});

	colourPickerButton->onClick([this,&gui]() {
		ToggleClourPicker(gui);
		});
	button->onPress([this,editBoxEquation, editBoxResolution, slider, color]() {
	std::string equationOutput = editBoxEquation->getText().toStdString();
	float resolution = editBoxResolution->getText().toFloat();
	float gridSize = slider->getValue();

	Graph(equationOutput, resolution, gridSize, color);
	});

	MenuBar->connectMenuItem("Login", "Account Page", [this, &gui]() {
		gui.removeAllWidgets();
		ViewSelectionMenu(gui);
		});
}

void UserInterface::Render() 
{
	renderer.matProj = renderer.maths.DefineProjectionMatrix(window.getSize().y, window.getSize().x);
	renderer.OnUserUpdate();
	renderer.visibleGrid = gridVisible;
	if (colourPicker != NULL)
		renderer.baseColour = colourPicker->getColor();
}

bool UserInterface::run(tgui::BackendGui& gui) {

	try
	{
		auto blackTheme = tgui::Theme::create("TGUI-1.5/themes/Dark.txt");
		blackTheme->setDefault("TGUI-1.5/themes/Dark.txt");
		MainMenu(gui);
		return true;
	}
	catch (const tgui::Exception& e)
	{
		std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
		return false;
	}
}
