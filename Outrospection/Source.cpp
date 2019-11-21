#include <iostream>
#include "Shader.h"
#include "ObjectGeneral.h"
#include "stb_image.h"
#include "Camera.h"
#include "Model.h"
#include "Scene.h"
#include "Outrospection.h"

// instance of the game
Outrospection instance = Outrospection();

Outrospection getOutrospection() {
	return instance;
}

int main()
{
	instance.run();
	return 0;
}