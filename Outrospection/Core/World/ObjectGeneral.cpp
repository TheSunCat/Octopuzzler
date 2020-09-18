#include "ObjectGeneral.h"
#include "Core.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Rendering/ModelLoader.h"
#include "Core/Rendering/Shader.h"

ObjectGeneral::ObjectGeneral(const std::string& _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale)
{
    name = _name;
    pos = _pos;
    rot = _rot;
    scale = _scale;

    std::string modelPath = "./res/ObjectData/" + _name + "/" + _name + ".omd";

    ModelLoader modelLoader;

    if (!modelLoader.loadFile(modelPath))
    {
        LOG_ERROR("Failed to load %s!", modelPath);
        return;
    }

    meshes.reserve(modelLoader.loadedMeshes.size());

    while (!modelLoader.loadedMeshes.empty())
    {
        const Mesh& md = modelLoader.loadedMeshes.front();

        meshes.emplace_back(md);

        modelLoader.loadedMeshes.pop_front();
    }
}

void ObjectGeneral::draw(const Shader& shader) const
{
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

    for (const Mesh& m : meshes)
    {
        m.draw();
    }
}

glm::vec3 ObjectGeneral::getPos() const
{
    return pos;
}

glm::vec3 ObjectGeneral::getRot() const
{
    return rot;
}

glm::vec3 ObjectGeneral::getScale() const
{
    return scale;
}
