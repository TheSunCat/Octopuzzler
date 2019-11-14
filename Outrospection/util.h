#pragma once
#include <string>
#include <vector>
using namespace std;
vector<string> split(string input, string delimiter);

template <typename T>
void push_all(vector<T> input, vector<T> add);