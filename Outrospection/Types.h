#pragma once

#include <unordered_map>
#include <array>

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

struct CollisionPackage
{
    glm::vec3 eRadius; // elipsoid radius

    glm::vec3 R3Velocity;
    glm::vec3 R3Position;

    // info in eSpace
    glm::vec3 velocity;
    glm::vec3 normalizedVelocity;
    glm::vec3 basePoint;

    // Collision info
    bool foundCollision;
    float nearestDistance;
    glm::vec3 intersectionPoint;

};

struct AABB
{
    AABB() : AABB(glm::vec3(-0.5f), glm::vec3(0.5f)) {} // default to 1x1x1 bbox

    AABB(glm::vec3 _min, glm::vec3 _max) : min(_min), max(_max),
                                           center((_min + _max) / 2.0f),
                                           size(max - min),
                                           halfSize(size / 2.0f) {}

    glm::vec3 min;
    glm::vec3 max;

    glm::vec3 center;
    glm::vec3 size; // aka extent
    glm::vec3 halfSize;

    AABB operator* (const glm::mat4& a) const
    {
        AABB tmp(glm::vec4(min, 1) * a, glm::vec4(max, 1) * a);

        return tmp;
    }

    void operator*= (const glm::mat4& a)
    {
        min = glm::vec4(min, 1) * a;
        max = glm::vec4(min, 1) * a;

        center = (min + max) / 2.0f;
        size = max - min;
        halfSize = size / 2.0f;
    }
};



struct Plane
{
    Plane() = default;

    // Ax + By + Cz + D = 0
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

    Plane(float eqA, float eqB, float eqC, float eqD)
    {
        float len = sqrt(eqA * eqA + eqB * eqB + eqC * eqC);

        equation[0] = eqA / len;
        equation[1] = eqB / len;
        equation[2] = eqC / len;
        equation[3] = eqD / len;

        n = glm::vec3(equation[0], equation[1], equation[2]);
        orig = glm::vec3(0, -equation[3] / equation[1], 0); // if X and Z = 0, find Y
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
