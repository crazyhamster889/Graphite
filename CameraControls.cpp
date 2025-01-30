#include "CameraControls.h"
#include <TGUI/Backend/SFML-Graphics.hpp>

void CameraControls::DefineCameraPosition() {

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
