#include "ObjectGeneral.h"

#include "Rendering/ModelLoader.h"
#include "Rendering/TextureManager.h"

#include "../Source.h"

ObjectGeneral::ObjectGeneral(std::string _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale)
{
	name = _name;
	pos = _pos;
	rot = _rot;
	scale = _scale;

	std::string modelPath = "./res/ObjectData/" + _name + "/" + _name + ".omd";
	
	ModelLoader modelLoader;

	if (!modelLoader.loadFile(modelPath)) {
		std::cout << "Failed to load " << modelPath << "!" << std::endl;
		return;
	}

	TextureManager* _textureManager = &(getOutrospection()->textureManager);

	for (MeshData md : modelLoader.loadedMeshes) {
		Resource r("Textures/", md.meshMaterial.mapDiffuse);

		SimpleTexture meshTex = _textureManager->loadTexture(r);

		Mesh newMesh(md.meshName, md.vertices, md.indices, meshTex);

		meshes.push_back(newMesh);
	}
}

void ObjectGeneral::draw(const Shader& shader) {
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

	for (Mesh m : meshes)
	{
		m.draw(shader);
	}
}

glm::vec3 ObjectGeneral::getPos() {
	return pos;
}

glm::vec3 ObjectGeneral::getRot() {
	return rot;
}

glm::vec3 ObjectGeneral::getScale() {
	return scale;
}