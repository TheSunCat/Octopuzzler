#pragma once
#include <string>

enum AnimType {
	walk,
	idle,
	jump
};

struct Animation {
	AnimType animType;
	unsigned int frameCount;

	unsigned int frameLength;
	unsigned int frameTally;

	unsigned int curFrame;
	std::vector<unsigned int> frames;
};