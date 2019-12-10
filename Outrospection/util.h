#pragma once

#include <glm/glm.hpp>
#include <GLAD\glad.h>
#include "stb_image.h"
#include <vector>
#include <string>

std::vector<std::string> split(std::string input, std::string delimiter);

template <typename T>
void push_all(vector<T> &input, vector<T> &add) {
	for (T o : add)
		input.push_back(o);
}

glm::vec3 vecFromYaw(float yawDeg);

unsigned int TextureFromFile(const char* path, const std::string& directory);

void subTextureFromFile(const char* path, const std::string& directory, unsigned int parentTexture);