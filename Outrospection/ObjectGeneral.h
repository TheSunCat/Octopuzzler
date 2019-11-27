#ifndef OBJ_GENERAL_H
#define OBJ_GENERAL_H

#include <string>
#include "Model.h"
#include <glm/glm.hpp>

using namespace std;

class ObjectGeneral {
public:
	ObjectGeneral(string _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale) : model("./res/ObjectData/" + _name + "/" + _name + ".dae")
	{
		name = _name;
		pos = _pos;
		rot = _rot;
		scale = _scale;
	}

	void draw(const Shader& shader) {
		glm::mat4 modelMat = glm::mat4(1.0f);

		// Scale model
		modelMat = glm::scale(modelMat, scale);
		
		// rotate each axis individually bc ogl dum
		modelMat = glm::rotate(modelMat, glm::radians(rot.x), glm::vec3(1, 0, 0));
		modelMat = glm::rotate(modelMat, glm::radians(rot.y), glm::vec3(0, 1, 0));
		modelMat = glm::rotate(modelMat, glm::radians(rot.z), glm::vec3(0, 0, 1));

		// Translate model
		modelMat = glm::translate(modelMat, pos);

		shader.setMat4("model", modelMat);
		model.Draw(shader);
	}

	glm::vec3 getPos() {
		return pos;
	}

	glm::vec3 getRot() {
		return rot;
	}

	glm::vec3 getScale() {
		return scale;
	}

private:
	glm::vec3 pos, rot, scale;
	string name;
	Model model;
};

#endif