#pragma once
class Controller
{
public:
	bool jump;
	bool talk;
	bool pause;

	// L stick
	float leftForward;
	float leftSide;

	// R stick
	float rightForward;
	float rightSide;
};