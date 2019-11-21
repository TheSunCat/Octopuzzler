#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "ObjectGeneral.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Model.h"
#include "Scene.h"
#include "Outrospection.h"

// settings
const unsigned int SCR_WIDTH = 1152;
const unsigned int SCR_HEIGHT = 648;

// timing
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// camera stuff
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_HEIGHT / 2, lastY = SCR_WIDTH / 2;
bool firstMouse = true;

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