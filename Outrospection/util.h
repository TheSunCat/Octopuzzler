#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
using namespace std;
vector<string> split(string input, string delimiter);

template <typename T>
void push_all(vector<T> input, vector<T> add);

glm::vec3 vecFromYaw(float yawDeg);