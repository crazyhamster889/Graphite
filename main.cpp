
#include <cmath>
#include <execution>
#include <algorithm>
#include <string>
#include <vector>
#include <winuser.rh>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <iostream>
#include <thread>
#include <Windows.h>

#include "CMathParser.h"
#include "Database.h"


using namespace std;

const float ScreenWidth = 1500;
const float ScreenHeight = 1000;

sf::VertexArray graph(sf::Triangles);

bool visibleGrid = false;
tgui::Color baseColour;

DatabaseClass database;

class MathsHelpers
{
public:
	struct vec3d
	{
		float x, y, z;
	};

	struct triangle
	{
		vec3d p[3];

		float light;
		bool grid;
		sf::Color color;
	};

	struct mesh
	{
		vector<triangle> tris;
	};

	struct mat4x4
	{
		float m[4][4] = { 0 };
	};

	void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
	{
		o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
		o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
		o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
		float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

		if (w != 0.0f)
		{
			o.x /= w; o.y /= w; o.z /= w;
		}
	}

	mat4x4 MultiplyMatrix(mat4x4& i, mat4x4& o)
	{
		mat4x4 input;

		for (int rows = 0; rows <= 3; rows++)
		{
			float total = 0;

			for (int column = 0; column <= 3; column++)
			{
				for (int step = 0; step <= 3; step++)
				{
					total += i.m[step][rows] * o.m[column][step];
				}
				input.m[column][rows] = total;
				total = 0;
			}
		}
		return input;
	}

	mat4x4 matRotZ(float zRot)
	{
		mat4x4 temp;

		temp.m[0][0] = cosf(0); 
		temp.m[1][0] = -sinf(0); 
		temp.m[2][0] = 0;
		temp.m[0][1] = sinf(0); 
		temp.m[1][1] = cosf(0); 
		temp.m[2][1] = 0;
		temp.m[0][2] = 0; 
		temp.m[1][2] = 0; 
		temp.m[2][2] = 1;
		temp.m[3][3] = 1;
		return temp;
	}
	mat4x4 matRotY (float yRot)
	{
		mat4x4 temp;

		temp.m[0][0] = cosf(yRot);
		temp.m[0][2] = -sinf(yRot);
		temp.m[1][0] = 0;
		temp.m[1][1] = 1;
		temp.m[2][0] = sinf(yRot);
		temp.m[2][2] = cosf(yRot);
		temp.m[3][3] = 1;
		return temp;
	}
	mat4x4 matRotX(float xRot) 
	{
		mat4x4 temp;

		temp.m[0][0] = 1; 
		temp.m[1][0] = 0; 
		temp.m[2][0] = 0;
		temp.m[0][1] = 0; 
		temp.m[1][1] = cosf(xRot); 
		temp.m[2][1] = -sinf(xRot);
		temp.m[0][2] = 0; 
		temp.m[1][2] = sinf(xRot); 
		temp.m[2][2] = cosf(xRot);
		temp.m[3][3] = 1;
		return temp;
	}
};

MathsHelpers::mesh meshes[2];
MathsHelpers::mat4x4 matProj;

class Algorithms
{
public:
	string toLower(const string& s) {
		string lower = s;
		transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
		return lower;
	}

	string replaceAll(const string& str, const string& from, const string& to) {
		if (from.empty()) return str;
		string result = str;
		string lowerResult = toLower(str); 
		string lowerFrom = toLower(from);  

		size_t start_pos = 0;
		while ((start_pos = lowerResult.find(lowerFrom, start_pos)) != string::npos) {
			result.replace(start_pos, lowerFrom.length(), to);
			start_pos += to.length();
			lowerResult = toLower(result);
		}
		return result;
	}


	static float averageZ(const MathsHelpers::triangle& t) {
		return (t.p[0].z + t.p[1].z + t.p[2].z) / 3.0f;
	}

	static bool compareTriangles(const MathsHelpers::triangle& t1, const MathsHelpers::triangle& t2) {
		return averageZ(t1) > averageZ(t2);
	}

	static void merge(vector<MathsHelpers::triangle>& vec, int left, int mid, int right) {
		int n1 = mid - left + 1;
		int n2 = right - mid;

		vector<MathsHelpers::triangle> leftHalf(vec.begin() + left, vec.begin() + mid + 1);
		vector<MathsHelpers::triangle> rightHalf(vec.begin() + mid + 1, vec.begin() + right + 1);

		int i = 0, j = 0, k = left;
		while (i < n1 && j < n2) {
			if (compareTriangles(leftHalf[i], rightHalf[j])) {
				vec[k] = leftHalf[i];
				i++;
			}
			else {
				vec[k] = rightHalf[j];
				j++;
			}
			k++;
		}

		while (i < n1) {
			vec[k] = leftHalf[i];
			i++;
			k++;
		}

		while (j < n2) {
			vec[k] = rightHalf[j];
			j++;
			k++;
		}
	}

	// Recursive merge sort with parallelization
	static void parallelMergeSort(vector < MathsHelpers::triangle >& vec, int left, int right, int depth = 0) {
		if (left < right) {
			int mid = left + (right - left) / 2;

			if (depth < log2(thread::hardware_concurrency())) {
				thread leftThread(parallelMergeSort, ref(vec), left, mid, depth + 1);
				parallelMergeSort(vec, mid + 1, right, depth + 1);
				leftThread.join();
			}
			else {
				parallelMergeSort(vec, left, mid, depth + 1);
				parallelMergeSort(vec, mid + 1, right, depth + 1);
			}

			merge(vec, left, mid, right);
		}
	}

};

class CameraControls {
public:
	float xRot = 0.0f;
	float yRot = 0.0f;

	float xInitial = 0.0f;
	float yInitial = 0.0f;

	float xRotInitial = 0.0f;
	float yRotInitial = 0.0f;
	bool InitialClick = false;

	void DefineCameraPosition() {

		sf::Vector2i position = sf::Mouse::getPosition();

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

			if (InitialClick == false)
			{
				yRotInitial = yRot;
				xRotInitial = xRot;
				xInitial = position.x;
				yInitial = -position.y;
				InitialClick = true;
			}
			xRot = (xRotInitial)+(yInitial + position.y) / 1000;
			yRot = (yRotInitial)+(xInitial - position.x) / 1000;
		}
		else
		{
			InitialClick = false;
		}
	}
};

class Renderer {

	public:
		sf::RenderWindow& window;

	Renderer(sf::RenderWindow& targetWindow) : window(targetWindow){}
	CameraControls controls;
	MathsHelpers maths;

	void FillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, sf::RenderWindow& window)
	{
		sf::ConvexShape convex;

		// resize it to 5 points
		convex.setPointCount(3);

		// define the points
		convex.setPoint(0, sf::Vector2f(x1, y1));
		convex.setPoint(1, sf::Vector2f(x2, y2));
		convex.setPoint(2, sf::Vector2f(x3, y3));

		window.draw(convex);

		return;
	}

	void OnUserUpdate()
	{

	 	MathsHelpers::mat4x4 matRotX, matRotY, matRotZ, output;
		controls.DefineCameraPosition();

		matRotX = maths.matRotX(controls.xRot);
		matRotY = maths.matRotY(controls.yRot);
		matRotZ = maths.matRotZ(0);

		output = maths.MultiplyMatrix(matRotX, matRotY);
		output = maths.MultiplyMatrix(output, matRotZ);

		vector<MathsHelpers::triangle> vecTrianglesToRaster;


		for (int i = 0; i <= (end(meshes) - begin(meshes)) - 1; i++)
		{
			for (auto tri : meshes[i].tris)
			{

				MathsHelpers::triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

				maths.MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], output);
				maths.MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], output);
				maths.MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], output);

				triTranslated = triRotatedZ;
				triTranslated.p[0].z = triRotatedZ.p[0].z + 30.0f;
				triTranslated.p[1].z = triRotatedZ.p[1].z + 30.0f;
				triTranslated.p[2].z = triRotatedZ.p[2].z + 30.0f;

				MathsHelpers::vec3d normal, line1, line2;
				line1.x = tri.p[1].x - tri.p[0].x;
				line1.y = tri.p[1].y - tri.p[0].y;
				line1.z = tri.p[1].z - tri.p[0].z;

				line2.x = tri.p[2].x - tri.p[0].x;
				line2.y = tri.p[2].y - tri.p[0].y;
				line2.z = tri.p[2].z - tri.p[0].z;

				normal.x = line1.y * line2.z - line1.z * line2.y;
				normal.y = line1.z * line2.x - line1.x * line2.z;
				normal.z = line1.x * line2.y - line1.y * line2.x;

				float magnitude = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
				normal.x /= magnitude; normal.y /= magnitude; normal.z /= magnitude;

				if (i == 0)
				{
					MathsHelpers::vec3d light_direction = { -0.5f, -0.5f, 0.0f };
					float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
					light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

					float dp = light_direction.x * normal.x + light_direction.y * normal.y + light_direction.z * normal.z;
					float result = std::fmax(0.0f, std::fmin(dp, 0.8f));
					triProjected.color = baseColour;


					triProjected.light = result;
				}
				else
				{
					triProjected.color = tri.color;
					triProjected.light = 1;
				}


				maths.MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
				maths.MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
				maths.MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

				triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
				triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
				triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;


				triProjected.p[0].x *= 0.5f * (float)ScreenWidth;
				triProjected.p[0].y *= 0.5f * (float)ScreenHeight;
				triProjected.p[1].x *= 0.5f * (float)ScreenWidth;
				triProjected.p[1].y *= 0.5f * (float)ScreenHeight;
				triProjected.p[2].x *= 0.5f * (float)ScreenWidth;
				triProjected.p[2].y *= 0.5f * (float)ScreenHeight;

				if (i != 0)
					triProjected.grid = true;
				else
					triProjected.grid = false;

				vecTrianglesToRaster.push_back(triProjected);
			}
		}
		Algorithms algorithms;
		algorithms.parallelMergeSort(vecTrianglesToRaster, 0, vecTrianglesToRaster.size() - 1);

		window.clear(sf::Color::White);
		graph.clear();
		vector<sf::Vertex> gridVertices;

		int gridVertex = 0;

		for (auto& triProjected : vecTrianglesToRaster)
		{
			if (abs(triProjected.p[0].x) < ScreenWidth && abs(triProjected.p[0].y) < ScreenHeight || triProjected.grid) {
				if (abs(triProjected.p[1].x) < ScreenWidth && abs(triProjected.p[1].y) < ScreenHeight || triProjected.grid) {
					if (abs(triProjected.p[2].x) < ScreenWidth && abs(triProjected.p[2].y) < ScreenHeight || triProjected.grid) {

						sf::Vertex v1(sf::Vector2f(triProjected.p[0].x, triProjected.p[0].y));
						sf::Vertex v2(sf::Vector2f(triProjected.p[1].x, triProjected.p[1].y));
						sf::Vertex v3(sf::Vector2f(triProjected.p[2].x, triProjected.p[2].y));
						v2.color = sf::Color(triProjected.color.r * triProjected.light, triProjected.color.g * triProjected.light, triProjected.color.b * triProjected.light, triProjected.color.a);
						v1.color = sf::Color(triProjected.color.r * triProjected.light, triProjected.color.g * triProjected.light, triProjected.color.b * triProjected.light, triProjected.color.a);
						v3.color = sf::Color(triProjected.color.r * triProjected.light, triProjected.color.g * triProjected.light, triProjected.color.b * triProjected.light, triProjected.color.a);
						if (!triProjected.grid)
						{
							graph.append(v1);
							graph.append(v2);
							graph.append(v3);
						}
						else
						{
							gridVertices.push_back(v1);
							gridVertices.push_back(v2);

							gridVertices.push_back(v2);
							gridVertices.push_back(v3);

							gridVertices.push_back(v3);
							gridVertices.push_back(v1);
						}
					}
				}
			}

		}
		sf::RectangleShape grid;

		if (visibleGrid) {
			for (int i = 0; i < (end(gridVertices) - begin(gridVertices)) - 2; i += 2)
			{
				grid.setPosition(gridVertices[i].position);
				float angle = atan2(gridVertices[i].position.y - gridVertices[i + 1].position.y, gridVertices[i].position.x - gridVertices[i + 1].position.x);
				grid.setRotation(angle * 180 / PI);
				grid.setSize(sf::Vector2f(-sqrt(powf(gridVertices[i].position.y - gridVertices[i + 1].position.y, 2) + powf(gridVertices[i].position.x - gridVertices[i + 1].position.x, 2)), 0));

				grid.setOutlineColor(gridVertices[i].color);
				grid.setOutlineThickness(1);
				window.draw(grid);
			}
		}
		window.draw(graph);

		if (visibleGrid) {
			for (int i = 0; i < (end(gridVertices) - begin(gridVertices)) - 2; i += 2)
			{
				grid.setPosition(gridVertices[i].position);
				float angle = atan2(gridVertices[i].position.y - gridVertices[i + 1].position.y, gridVertices[i].position.x - gridVertices[i + 1].position.x);
				grid.setRotation(angle * 180 / PI);
				grid.setSize(sf::Vector2f(-sqrt(powf(gridVertices[i].position.y - gridVertices[i + 1].position.y, 2) + powf(gridVertices[i].position.x - gridVertices[i + 1].position.x, 2)), 0));

				grid.setOutlineColor({ gridVertices[i].color.r, gridVertices[i].color.g, gridVertices[i].color.b, 10 });
				grid.setOutlineThickness(1);
				window.draw(grid);
			}
		}
		return;
	}
};

class BuildGraph
{
public:

	float gridSize = 6;

	void CreateQuad(MathsHelpers::vec3d point1, MathsHelpers::vec3d point2, MathsHelpers::vec3d point3, MathsHelpers::vec3d point4, int ID, sf::Color Color)
	{
		meshes[ID].tris.push_back({ point2,    point3,    point1, 0, false, Color });
		meshes[ID].tris.push_back({ point4,    point3,    point2, 0, false, Color });
	}

	void CreateGridLine(MathsHelpers::vec3d Point1, MathsHelpers::vec3d Point2, sf::Color GridColour)
	{
		CreateQuad(Point1, Point1, Point2, Point2, 1, GridColour);
	}

	void GridBuilder()
	{
		sf::Color GridColour = { 150,150,150,60 };

		for (float j = -gridSize; j <= gridSize; j += 0.7)
		{
			if (j == 0)
				continue;
			CreateGridLine({ -gridSize, 1, j }, { gridSize, 1, j }, GridColour);
		}
		for (float k = -gridSize; k <= gridSize; k += 0.7)
		{
			if (k == 0)
				continue;
			CreateGridLine({ k, 1, -gridSize }, { k,1,gridSize }, GridColour);
		}
		CreateGridLine({ 0, 1, 0 }, { 0, -gridSize, 0 }, sf::Color::Blue);
		CreateGridLine({ -gridSize, 1, 0 }, { gridSize, 1, 0 }, sf::Color::Red);
		CreateGridLine({ 0, 1, -gridSize }, { 0, 1, gridSize }, sf::Color::Green);

		CreateGridLine({ gridSize, 1, gridSize }, { gridSize, -gridSize, gridSize }, GridColour);
		CreateGridLine({ gridSize, 1, -gridSize }, { gridSize, -gridSize, -gridSize }, GridColour);
		CreateGridLine({ -gridSize, 1, -gridSize }, { -gridSize, -gridSize, -gridSize }, GridColour);
		CreateGridLine({ -gridSize, 1, gridSize }, { -gridSize, -gridSize, gridSize }, GridColour);

		CreateGridLine({ -gridSize, -gridSize, gridSize }, { gridSize, -gridSize, gridSize }, GridColour);
		CreateGridLine({ gridSize, -gridSize, gridSize }, { gridSize, -gridSize, -gridSize }, GridColour);
		CreateGridLine({ gridSize, -gridSize, -gridSize }, { -gridSize, -gridSize, -gridSize }, GridColour);
		CreateGridLine({ -gridSize, -gridSize, -gridSize }, { -gridSize, -gridSize, gridSize }, GridColour);
	}

	void OnUserCreate(string equationInput, tgui::EditBox::Ptr resolutionInput)
	{
		for (int i = 0; i <= (end(meshes) - begin(meshes)) - 1; i++)
		{
			meshes[i].tris.clear();
		}

		float size = resolutionInput->getText().toFloat();

		string equation = equationInput;
		database.InsertIntoDatabase(*equation.data(), *equation.data());
		parser ob;
		Algorithms algorithms;
		ob.eval_exp("z = " + to_string(size));

		for (float x = -gridSize; x <= gridSize; x += size)
		{
			ob.eval_exp("x = " + to_string(x));

			for (float y = -gridSize; y <= gridSize; y += size)
			{
				ob.eval_exp("y = " + to_string(y));

				string ZXExpression = algorithms.replaceAll(equation, "y", "(Y-Z)");
				ZXExpression = algorithms.replaceAll(ZXExpression, "x", "(X-Z)");

				float zXOld = -ob.eval_exp(algorithms.replaceAll(equation, "x", "(X-Z)"));
				float zXYOld = -ob.eval_exp(ZXExpression);
				float zYOld = -ob.eval_exp(algorithms.replaceAll(equation, "y", "(Y-Z)"));
				float z = -ob.eval_exp(equation);

				if ((z < gridSize && z > -gridSize) || (zXYOld < gridSize && zXYOld > -gridSize) || (zYOld < gridSize && zYOld > -gridSize) || (zXOld < gridSize && zXOld > -gridSize)) {
					zXOld = std::fmax(-gridSize, std::fmin(zXOld, gridSize));
					zXYOld = std::fmax(-gridSize, std::fmin(zXYOld, gridSize));
					zYOld = std::fmax(-gridSize, std::fmin(zYOld, gridSize));
					z = std::fmax(-gridSize, std::fmin(z, gridSize));
					CreateQuad({ x + -size / 2.0f, zXYOld, y - size / 2.0f }, { x + size / 2.0f, zYOld, y - size / 2.0f }, { x + -size / 2.0f, zXOld, y + size / 2.0f }, { x + size / 2.0f, z, y + size / 2.0f }, 0, baseColour);
				}
			}
		}

		GridBuilder();

		float nearPlane = 10.0f;
		float farPlane = 100.0f;
		float fov = 40.0f;
		float aspectRatio = (float)ScreenHeight / (float)ScreenWidth;
		float fovRad = 1.0f / tanf(fov * 0.5f / 180 * 3.14159f);

		matProj.m[0][0] = aspectRatio * fovRad;
		matProj.m[1][1] = fovRad;
		matProj.m[2][2] = farPlane / (farPlane - nearPlane);
		matProj.m[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;

		return;
	}
};

void updateTextSize(tgui::BackendGui& gui)
{
	// Update the text size of all widgets in the gui, based on the current window height
	const float windowHeight = gui.getView().getRect().height;
	gui.setTextSize(0.02f * windowHeight); // 7% of height
}

tgui::ColorPicker::Ptr colourPicker = nullptr;

void ToggleGrid()
{
	visibleGrid = !visibleGrid;
}
void ToggleClourPicker(tgui::BackendGui& gui)
{
	gui.add(colourPicker);
}

void TempFunction(tgui::EditBox::Ptr equationInput, tgui::EditBox::Ptr resolutionInput, tgui::Slider::Ptr sliderInput) {
	BuildGraph graph;
	graph.gridSize = sliderInput->getValue();

	string equationContents = (string)equationInput->getText();
	//cout << equationContents.empty();
	if (equationContents.empty() != 1)
	{
		graph.OnUserCreate(equationContents, resolutionInput);
	}
	else 
	{
		graph.OnUserCreate(database.LastEquation(), resolutionInput);
	}

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
    baseColour = colourPicker->getColor();

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
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(ScreenWidth, ScreenHeight), "I LOVE BIRDS", sf::Style::Resize, settings);
	window.setFramerateLimit(1000);
	window.setVerticalSyncEnabled(true);
	tgui::Gui gui{ window };
	run(gui);

	Renderer renderer (window);
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

		renderer.OnUserUpdate();
		baseColour = colourPicker->getColor();
		cout << colourPicker->getColor().getBlue();
		gui.draw();
		window.display();
	}

    return 0;
}


