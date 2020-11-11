#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

#include "Core/Rendering/Resource.h"
#include "Core/Rendering/SimpleTexture.h"

//enum class AnimType {
//    walk,
//    idle,
//    jump, 
//    fall
//};
//
//const std::unordered_map<AnimType, std::string> animTypeMap ({
//    {AnimType::idle, "idle"},
//    {AnimType::walk, "walk"},
//    {AnimType::jump, "jump"},
//    {AnimType::fall, "fall"}
//});

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;

    glm::vec2 texCoord;

    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Triangle
{
    //Triangle (const Vertex& a, const Vertex& b, const Vertex& c)
    //{
    //    verts[0] = a.pos;
    //    verts[1] = b.pos;
    //    verts[2] = c.pos;

    //    n = (a.normal + b.normal + c.normal) / 3.0f;
    //}

    glm::vec3 verts[3];
    glm::vec3 n;
};

struct Plane
{
    float equation[4]{};

    // two things needed to define any plane:
    glm::vec3 orig{};
    glm::vec3 n{};

    Plane(const glm::vec3& origin, const glm::vec3& normal)
    {
        orig = origin;
        n = normal;

        equation[0] = n.x;
        equation[1] = n.y;
        equation[2] = n.z;
        equation[3] = -(n.x * orig.x + n.y * orig.y + n.z * orig.z);
    }


    Plane(const Triangle& tri)
    {
        orig = tri.verts[0];
        n = glm::normalize(glm::cross(tri.verts[1] - tri.verts[0], tri.verts[2] - tri.verts[0]));

        equation[0] = n.x;
        equation[1] = n.y;
        equation[2] = n.z;
        equation[3] = -(n.x * orig.x + n.y * orig.y + n.z * orig.z);
    }

    bool isFrontFacingTo(const glm::vec3& dir) const
    {
        float dot = glm::dot(n, dir);
        return dot <= 0.0f;
    }

    float signedDistanceTo(const glm::vec3& point) const
    {
        return float(glm::dot(point, n)) + equation[3];
    }
};

struct Ray
{
    glm::vec3 origin; // where the ray starts
    glm::vec3 direction; // direction the ray is going in
};

struct Collision
{
    float dist;
    Triangle tri;
};

struct FontCharacter
{
    GLuint textureId;
    glm::ivec2 size;
    glm::ivec2 bearing; // offset from base line
    long advance; // offset to advance to next glyph
};

struct DummyObj
{
    std::string name;
    std::vector<float> properties;
};

typedef glm::vec3 Color;

class Hashes
{
public:
    size_t operator()(const SimpleTexture& st) const
    {
        return st.texId;
    }

    size_t operator()(const Resource& r) const
    {
        const std::hash<std::string> strhash;

        return strhash(r.fullPath);
    }
};
