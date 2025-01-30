#include <vector>
#include "MathsUtils.h"
#include <TGUI/Backend/SFML-Graphics.hpp>


#ifndef CAMERACONTROLS_H
#define CAMERACONTROLS_H

using namespace std;

class CameraControls
{
public:
	float xRot = 0.0f;
	float yRot = 0.0f;

	float xInitial = 0.0f;
	float yInitial = 0.0f;

	float xRotInitial = 0.0f;
	float yRotInitial = 0.0f;
	bool InitialClick = false;

	void DefineCameraPosition();
};

#endif 