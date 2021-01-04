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

glm::mat4 MVP{};

void ObjectGeneral::draw(const Shader& shader, const Camera& cam) const
{
    if (hidden)
        return;
    
    if (dirtyTransform)
    {
        modelMat = glm::mat4(1.0f);

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

    //MVP = cam.viewProj * modelMat;

    for (const Mesh& m : meshes)
    {
        // TODO frustum culling :c
        //auto bbox = m.bbox * modelMat;

        //bool inFrustum = Util::intersectFrustumAABB(cam.frustum, bbox);

        //if (!inFrustum)
        //{
        //    Outrospection::culled++;
        //    return;
        //}


        m.draw();
    }
}

const glm::vec3& ObjectGeneral::getPos() const
{
    return pos;
}

glm::vec3 ObjectGeneral::getRot() const
{
    return glm::degrees(rotRad);
}

const glm::vec3& ObjectGeneral::getRotRad() const
{
    return rotRad;
}

const glm::vec3& ObjectGeneral::getScale() const
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
