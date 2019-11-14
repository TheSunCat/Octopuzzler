#include "ObjectRail.h"

ObjectRail::ObjectRail(ObjectGeneral _obj) : obj(_obj)
{
	
}

void ObjectRail::draw(Shader shader) {
	obj.draw(shader);
}