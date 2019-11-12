#ifndef OBJECT_RAIL_H
#define OBJECT_RAIL_H

#include "ObjectGeneral.h"

using namespace std;

class ObjectRail {
public:
	ObjectRail(ObjectGeneral _obj)
	{
		obj = _obj;
	}

	void draw(Shader shader) {
		obj.draw(shader);
	}

private:
	ObjectGeneral obj;
};
//#endif