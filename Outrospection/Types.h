#pragma once
#include <string>

enum AnimType {
	walk,
	idle,
	jump
};

struct Animation {
	AnimType animType;
	unsigned int texId;
	unsigned int frameCount;
};