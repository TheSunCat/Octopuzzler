#pragma once

struct ControllerButton
{
	float framesDown;
};

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