#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

using namespace std;
vector<string> split(string input, string delimiter);

template <typename T>
void push_all(vector<T> &input, vector<T> &add) {
	for (T o : add)
		input.push_back(o);
}

glm::vec3 vecFromYaw(float yawDeg);