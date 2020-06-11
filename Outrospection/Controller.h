#pragma once

// stores the amount of frames the button is down, 0 if never
typedef unsigned int ControllerButton;

class Controller
{
public:
	bool isGamepad;

	// INPUT

	ControllerButton jump;
	ControllerButton talk;
	ControllerButton pause;

	ControllerButton debugBreak;
	
	// L stick
	float leftForward;
	float leftSide;

	// R stick
	float rightForward;
	float rightSide;

	// L trigger
	float leftTrigger;

};