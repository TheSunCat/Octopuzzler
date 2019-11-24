#include "Util.h"

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

glm::vec3 vecFromYaw(float yawDeg)
{
	glm::vec3 front;
	front.x = cos(glm::radians(yawDeg));
	front.y = 0;
	front.z = sin(glm::radians(yawDeg));
	front = glm::normalize(front);

	return front;
}