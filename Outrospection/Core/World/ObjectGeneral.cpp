#include "ObjectGeneral.h"
#include "Core.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "MeshSphere.h"
#include "Core/Rendering/ModelLoader.h"
#include "Core/Rendering/Shader.h"

ObjectGeneral::ObjectGeneral(const std::string& _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale)
{
    name = _name;
    setPos(_pos);
    setRot(_rot);
    setScale(_scale);

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

ObjectGeneral::ObjectGeneral(const std::string& _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale, Mesh& mesh)
{
    name = _name;
    setPos(_pos);
    setRot(_rot);
    setScale(_scale);

    meshes.emplace_back(mesh);
}

void ObjectGeneral::draw(const Shader& shader) const
{
    if (hidden)
        return;
    
    if (dirtyTransform)
    {
        modelMat = glm::mat4(1.0f);

        // TODO figure out the order to do this mess in

        // Translate model
        modelMat = glm::translate(modelMat, pos);

        // Rotate model by quaternion
        modelMat *= glm::toMat4(rot);

        // Scale model
        modelMat = glm::scale(modelMat, scale);

        
        dirtyTransform = false;
    }

    shader.setMat4("model", modelMat);
    shader.setFloat("debugColor", debugColor);

    for (const Mesh& m : meshes)
    {
        m.draw();
    }
}

glm::vec3& ObjectGeneral::getPos()
{
    return pos;
}

glm::vec3 ObjectGeneral::getRot() const
{
    return glm::degrees(rotRad);
}

glm::vec3& ObjectGeneral::getRotRad()
{
    return rotRad;
}

glm::vec3& ObjectGeneral::getScale()
{
    return scale;
}

void ObjectGeneral::setPos(const glm::vec3& _pos)
{
    pos = _pos;

    dirtyTransform = true;
}

void ObjectGeneral::setRot(const glm::vec3& _rot)
{
    rotRad = glm::radians(_rot);

    rot = glm::quat(rotRad);

    dirtyTransform = true;
}

void ObjectGeneral::setScale(const glm::vec3& _scl)
{
    scale = _scl;

    dirtyTransform = true;
}
