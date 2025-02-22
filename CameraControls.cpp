#include "CameraControls.h"
#include <TGUI/Backend/SFML-Graphics.hpp>

void CameraControls::DefineCameraPosition() {

	sf::Vector2i position = sf::Mouse::getPosition();

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		// sets the initial click position
		if (InitialClick == false)
		{
			yRotInitial = yRot;
			xRotInitial = xRot;
			xInitial = position.x;
			yInitial = -position.y;
			InitialClick = true;
		}

		/* 
		Calculate the new rotation based on the initial click position and the current mouse position,
		dividing by a sensitivity factor
		*/ 
		xRot = (xRotInitial)+(yInitial + position.y) / 1000 * (sensitivity);
		yRot = (yRotInitial)+(xInitial - position.x) / 1000 * (sensitivity);
	}
	else {InitialClick = false;}
}
