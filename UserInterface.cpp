#include "UserInterface.h"
#include "Database.h"
#include "BuildGraph.h"
#include <string>
#include <functional> 
#include <Windows.h>
using namespace std;

// UserInterface constructor, sets up the window, the graph, renderer, and the database
UserInterface::UserInterface(sf::RenderWindow& targetWindow, BuildGraph& targetGraph, DatabaseClass& database)
	: window(targetWindow), graph(targetGraph), databaseInstance(database), renderer(window, graph){}

// Toggles the grid visibility
void UserInterface::ToggleGrid() { gridVisible = !gridVisible; }
// Toggle light mode
void UserInterface::ToggleLightMode() 
{ 
	themeEnabled = !themeEnabled;
	cout << themeEnabled;
	if (themeEnabled)
		Theme->setDefault("TGUI-1.5/themes/Dark.txt");
	else 
		Theme->setDefault("TGUI-1.5/themes/Light.txt");
}
// Toggles the colour picker visibility
void UserInterface::ToggleClourPicker(tgui::BackendGui& gui) { gui.add(colourPicker); }

// Creates a graph with the given equation, resolution, slider value, and colour
void UserInterface::CreateGraph(string equationInput, float resolutionInput, float sliderInput, tgui::Color color)
{
	// ensures the resolution is not 0
	if (resolutionInput == 0) {
		MessageBox(NULL, L"No resolution specified", L"Error", MB_OK);
		return;
	}
	// equation format validation
	std::regex invalidPattern(R"([^0-9+\-*/^().a-zA-Z\s])");
	if (equationInput.empty() != 1  && !regex_search(equationInput, invalidPattern))
		graph.OnUserCreate(equationInput, resolutionInput, userID);
	else
		MessageBox(NULL, L"Invalid Equation", L"Error", MB_OK);

	renderer.graphConstructor = graph;
	renderer.baseColour = color;
}

// Main menu for the user interface
void UserInterface::MainMenuScreen(tgui::BackendGui& gui)
{
	// sets up the group to ensure that we can disable all the UI elements when switching screens
	auto menuGroup = tgui::Group::create();
	// creates all the UI elements with helper functions that I created to tidy the program
	tgui::Button::Ptr createNewProject = createButton("Create Project", { "30%", "10%" }, { "35%", "35%" }, menuGroup);
	tgui::Button::Ptr settings = createButton("Settings", { "30%", "10%" }, { "35%", "50%" }, menuGroup);
	tgui::Button::Ptr help = createButton("Help", { "30%", "10%" }, { "35%", "60%" }, menuGroup);
	tgui::Button::Ptr selectGraphs = createButton("Select Graphs", { "30%", "10%" }, { "35%", "70%" }, menuGroup);
	gui.add(menuGroup);

	settings->onClick([this, &gui, menuGroup]() {
		menuGroup->setVisible(false);
		rendererVisible = true;
		SettingsMenuScreen(gui); });

	createNewProject->onClick([this, &gui, menuGroup]() {
		rendererVisible = true;
		menuGroup->setVisible(false);
		GraphingScreen(gui); });

	selectGraphs->onClick([this, &gui, menuGroup]() {
		menuGroup->setVisible(false);
		ViewSelectionScreen(gui); });
}

void UserInterface::SettingsMenuScreen(tgui::BackendGui& gui)
{
	auto settingsGroup = tgui::Group::create();
	auto switchTheme = createCheckBox({ "30%","5%" }, { "2.5%", "25%" }, settingsGroup);
	switchTheme->onClick([this, &gui]() { ToggleLightMode(); gui.removeAllWidgets(); SettingsMenuScreen(gui); });
	auto sensitivity = createSlider({ "30%","3%" }, { "2.5%", "40%" }, settingsGroup);
	auto MenuBar = createMenuBar(gui, settingsGroup);

	tgui::Button::Ptr exit = createButton("Main Menu", { "30%", "10%" }, { "2.5%", "60%" }, settingsGroup);
	tgui::Button::Ptr graphing = createButton("Graphing Screen", { "30%", "10%" }, { "2.5%", "70%" }, settingsGroup);

	sensitivity->setMinimum(1);
	sensitivity->setMaximum(30);

	exit->onClick([this, &gui, settingsGroup]() {
		settingsGroup->setVisible(false);
		rendererVisible = false;
		MainMenuScreen(gui); 
		});

	graphing->onClick([this, &gui, settingsGroup]() {
		settingsGroup->setVisible(false);
		rendererVisible = true;
		GraphingScreen(gui);
		});

	sensitivity->onValueChange([this, &gui, sensitivity]() {
		renderer.controls.sensitivity = sensitivity->getValue()/10; });

	gui.add(settingsGroup);
}

// Selection menu for the user interface
void UserInterface::ViewSelectionScreen(tgui::BackendGui& gui)
{
	auto selectionMenuGroup = tgui::Group::create();

	// creates all the UI elements with helper functions that I created to tidy the program
	tgui::EditBox::Ptr usernameLogin = createEditBox("Username", { "35%", "5%" }, { "2%", "20%" }, selectionMenuGroup);
	tgui::EditBox::Ptr passwordLogin = createEditBox("Password", {"35%", "5%"}, {"2%", "30%"}, selectionMenuGroup);
	tgui::EditBox::Ptr classLogin = createEditBox("Class", {"15%", "5%"}, {"2%", "35%"}, selectionMenuGroup);
	tgui::EditBox::Ptr courseName = createEditBox("Course", {"15%", "5%"}, { "22%", "35%" }, selectionMenuGroup);
	tgui::EditBox::Ptr courseSubject = createEditBox("Course Subject", { "15%", "5%" }, { "22%", "40%" }, selectionMenuGroup);
	tgui::Button::Ptr createClass = createButton("Create Class", { "15%", "10%" }, { "2%", "45%" }, selectionMenuGroup);
	tgui::Button::Ptr createAccount = createButton("Create Account / Login", { "35%", "10%" }, { "2%", "55%" }, selectionMenuGroup);
	tgui::Button::Ptr createCourse = createButton("Create Course", { "15%", "10%" }, { "22%", "45%" }, selectionMenuGroup);
	tgui::Button::Ptr Graphing = createButton("Graphing page", { "35%", "10%" }, { "2%", "90%" }, selectionMenuGroup);
	tgui::ListBox::Ptr ListView = createListView({ "35%", "25%" }, { "2%", "65%" }, selectionMenuGroup);
	auto MenuBar = createMenuBar(gui, selectionMenuGroup);

	// sets up the callbacks for each button and the list view
	createCourse->onClick([this, courseSubject, courseName]() 
	{databaseInstance.InsertIntoCourseTable(*courseName->getText().toStdString().data(), *courseSubject->getText().toStdString().data());});
	createClass->onClick([this, classLogin, courseName]() 
	{databaseInstance.InsertIntoClassTable(*classLogin->getText().toStdString().data(), *courseName->getText().toStdString().data()); });
	createAccount->onClick([this, usernameLogin, passwordLogin, ListView, courseName, classLogin]()
	{
		userID = databaseInstance.InsertIntoUserTable(*usernameLogin->getText().toStdString().data(), 
													  *passwordLogin->getText().toStdString().data(),
													  *classLogin->getText().toStdString().data());
		ListView->removeAllItems();
		populateList(ListView, databaseInstance); });
	// changes screen by disabling the current group and enabling the graphing group
	Graphing->onClick([this, &gui, selectionMenuGroup]() {
		rendererVisible = true;
		selectionMenuGroup->setVisible(false);
		GraphingScreen(gui); });

	// ensures the current group is visible
	selectionMenuGroup->setVisible(true);
	gui.add(selectionMenuGroup);
}

void UserInterface::GraphingScreen(tgui::BackendGui& gui)
{
	auto graphingGroup = tgui::Group::create();
	auto interactionPanel = tgui::Panel::create();
	interactionPanel->setSize({ "27%", "100%" });
	interactionPanel->setPosition({ "0%", "0%" });

	graphingGroup->add(interactionPanel);
	// creates all the UI elements with helper functions that I created to tidy the program
	tgui::EditBox::Ptr editBoxEquation = createEditBox("Equation", { "20%", "5%" }, { "2%", "10%" }, graphingGroup);
	tgui::EditBox::Ptr editBoxResolution = createEditBox("Resolution", { "20%", "5%" }, { "2%", "15%" }, graphingGroup);
	tgui::Button::Ptr colourPickerButton = createButton("ƒ", { "3%", "5%" }, { "22%", "10%" }, graphingGroup);
	auto listView = createListView({ "20%", "50%" }, { "2%", "35%" }, graphingGroup);
	tgui::Button::Ptr graphButton = createButton("Graph", { "20%", "10%" }, { "2%", "85%" }, graphingGroup);

	auto toolBar = tgui::Panel::create({ "100%", "5%" });
	graphingGroup->add(toolBar);
	auto checkBox = createCheckBox({ "2%","3%" }, { "2%", "22%" }, graphingGroup);
	auto MenuBar = createMenuBar(gui, graphingGroup);
	auto slider = createSlider({ "20%","3%" }, { "2.5%", "30%" }, graphingGroup);

	// sets up the colour picker
	colourPicker = tgui::ColorPicker::create();
	colourPicker->setSize({ "40%", "30%" });
	colourPicker->setPosition({ "26%", "20%" });
	colourPicker->setPositionLocked(true);
	colourPicker->setColor(tgui::Color::Cyan);
	graphingGroup->add(colourPicker);
	colourPicker->close();

	// Toggle grid
	checkBox->onClick([this]() { ToggleGrid(); });

	// sets up the callbacks for each button and the list view
	listView->onItemSelect([this, listView, editBoxResolution, slider]() {
		CreateGraph(
			listView->getSelectedItem().toStdString(),
			editBoxResolution->getText().toFloat(),
			slider->getValue(),
			colourPicker->getColor()
		); });

	graphButton->onPress([this, editBoxEquation, editBoxResolution, slider]() {
		CreateGraph(
			editBoxEquation->getText().toStdString(),
			editBoxResolution->getText().toFloat(),
			slider->getValue(),
			colourPicker->getColor()
		); });

	// populates the list view with the equations 
	populateList(listView, databaseInstance);
	colourPickerButton->onClick([this,&gui]() {ToggleClourPicker(gui);});
	graphingGroup->setVisible(true);
	gui.add(graphingGroup);
}

// helper functions
void UserInterface::populateList(tgui::ListBox::Ptr listView, DatabaseClass databaseInstance) {
	for (string item : databaseInstance.FetchRecentEquations(userID))
	{
		listView->addItem(item);
	}
}

tgui::Button::Ptr UserInterface::createButton(const std::string& text, tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group)
{
	auto button = tgui::Button::create(text);
	button->setSize(size);
	button->setPosition(position);
	group->add(button);
	return button;
}

tgui::EditBox::Ptr UserInterface::createEditBox(const std::string& placeholder, tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group)
{
	auto editBox = tgui::EditBox::create();
	editBox->setSize(size);
	editBox->setPosition(position);
	editBox->setDefaultText(placeholder);
	group->add(editBox);
	return editBox;
}

tgui::Slider::Ptr UserInterface::createSlider(tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group)
{
	auto slider = tgui::Slider::create();
	slider->setSize(size);
	slider->setPosition(position);
	group->add(slider);
	return slider;
}

tgui::ListBox::Ptr UserInterface::createListView(tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group) {
	auto listView = tgui::ListBox::create();
	listView->setSize(size);
	listView->setPosition(position);
	group->add(listView);
	return listView;
}

tgui::CheckBox::Ptr UserInterface::createCheckBox(tgui::Layout2d size, tgui::Layout2d position, tgui::Group::Ptr& group)
{
	auto checkBox = tgui::CheckBox::create();
	checkBox->setSize(size);
	checkBox->setPosition(position);
	group->add(checkBox);
	return checkBox;
}

// sets up menu bar and the callbacks for each menu item
tgui::MenuBar::Ptr UserInterface::createMenuBar(tgui::BackendGui& gui, tgui::Group::Ptr& group)
{
	auto menuBar = tgui::MenuBar::create();
	menuBar->setSize({ "100%", "3%" });
	menuBar->setPosition({ "1%", "1%" });

	menuBar->addMenu("File");
	menuBar->addMenuItem("File", "Save");
	menuBar->addMenuItem("File", "Main Menu");
	menuBar->addMenuItem("File", "Exit");

	menuBar->addMenu("Settings");
	menuBar->addMenuItem("Settings", "Settings Page");

	menuBar->addMenu("Login");
	menuBar->addMenuItem("Login", "Account Page");

	menuBar->addMenu("Help");
	menuBar->setTextSize(20);

	menuBar->connectMenuItem("File", "Exit", [this, &gui]() {
		window.close();
		});

	menuBar->connectMenuItem("Login", "Account Page", [this, &gui, group]() {
		group->setVisible(false);
		ViewSelectionScreen(gui);
		});
	menuBar->connectMenuItem("File", "Main Menu", [this, &gui, group]() {
		group->setVisible(false);
		rendererVisible = false;
		MainMenuScreen(gui);
		});
	menuBar->connectMenuItem("Settings", "Settings Page", [this, &gui, group]() {
		group->setVisible(false);
		SettingsMenuScreen(gui);
		});
	group->add(menuBar);

	return menuBar;
}


void UserInterface::Render() 
{
	WINDOWINFO wiInfo;
	GetWindowInfo(window.getSystemHandle(), &wiInfo);

	float width = wiInfo.rcClient.right - wiInfo.rcClient.left;
	float height = wiInfo.rcClient.bottom - wiInfo.rcClient.top;

	// sets up the projection matrix for the renderer
	renderer.matProj = renderer.maths.DefineProjectionMatrix(height, width);

	if (rendererVisible)
		renderer.OnUserUpdate(width, height);
	else
		window.clear(sf::Color::White);

	// sets the grid visibility and the base colour for the renderer
	renderer.visibleGrid = gridVisible;

	if (colourPicker != NULL)
		renderer.baseColour = colourPicker->getColor();
}

bool UserInterface::run(tgui::BackendGui& gui) {
	// Try and catch to handle any errors when setting up the UI
	try
	{
		// sets up the theme for the UI, using the incredible UI config by finjosh
		ToggleLightMode();
		MainMenuScreen(gui);
		return true;
	}
	catch (const tgui::Exception& e)
	{
		std::cerr << "Failed to load TGUI widgets: " << e.what() << endl;
		return false;
	}
}
