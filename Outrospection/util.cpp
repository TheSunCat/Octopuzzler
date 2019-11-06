#include <string>
#include <vector>
#include "util.h"

using namespace std;
vector<string> split(string input, string delimiter) {
	vector<string>* toReturn = new vector<string>();
	bool finished = false;
	while (!finished) {
		int location = input.find(delimiter);
		if (location == string::npos) 
		{
			finished = true;
			toReturn->push_back(input);
		}
		else
		{
			toReturn->push_back(input.substr(0, location));
			input = input.substr(location + delimiter.length());
		}
	}
	return *toReturn;
}