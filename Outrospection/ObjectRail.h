#ifndef OBJECT_RAIL_H
#define OBJECT_RAIL_H

#include "ObjectGeneral.h"
#include "Shader.h"

class ObjectRail {
public:
	ObjectRail(ObjectGeneral _obj);

	void draw(Shader shader);

private:
	ObjectGeneral obj;
};

#endif