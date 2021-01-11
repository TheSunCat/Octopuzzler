#include "ObjectGeneral.h"
#include "Core.h"

#include "Core/Rendering/MeshSphere.h"
#include "Core/Rendering/ModelLoader.h"
#include "Core/Rendering/Shader.h"

ObjectGeneral::ObjectGeneral(const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Collider* _collider) : collider(_collider)
{
    name = "empty";
    transform.setPos(_pos);
    transform.setRot(_rot);
    transform.setScl(_scale);
}

ObjectGeneral::ObjectGeneral(const std::string& _name, const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Collider* _collider)
        : collider(_collider)
{
    name = _name;
    transform.setPos(_pos);
    transform.setRot(_rot);
    transform.setScl(_scale);

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

ObjectGeneral::ObjectGeneral(const std::string& _name, const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Mesh& mesh, Collider* _collider)
        : collider(_collider)
{
    name = _name;
    transform.setPos(_pos);
    transform.setRot(_rot);
    transform.setScl(_scale);

    meshes.emplace_back(mesh);
}

glm::mat4 MVP{};

void ObjectGeneral::draw(const Shader& shader, const Camera& cam) const
{
    if (hidden)
        return;

    shader.setMat4("model", transform.mat());
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