#pragma once

#include <json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/Rendering/Resource.h"
#include "Core/Rendering/SimpleTexture.h"

struct Level
{
    std::string data;
    int rowLength;
    std::string controls;

    glm::vec2 start;
    glm::vec2 goal;

    std::string guideRight;
    std::string guideLeft;

    std::string author;
};

inline void from_json(const nlohmann::json& j, Level& lvl)
{
    auto rows = j["level"].get<std::vector<std::string>>();
    lvl.rowLength = rows[0].size();
    lvl.data = std::accumulate(begin(rows), end(rows), std::string());

    j["controls"].get_to(lvl.controls);

    lvl.guideLeft = j.value("guideLeft", "default");
    lvl.guideRight = j.value("guideRight", "default");

    int startIndex = lvl.data.find('S'); lvl.data[startIndex] = ' ';
    lvl.start = glm::vec2(startIndex % lvl.rowLength, int(startIndex / lvl.rowLength));

    int goalIndex = lvl.data.find('G'); lvl.data[goalIndex] = ' ';
    lvl.goal = glm::vec2(goalIndex % lvl.rowLength, int(goalIndex / lvl.rowLength));

    lvl.author = j.value("author", "Anonymous");
}

enum class Control
{
    NONE = ' ',
    MOVE_UP = '*',
    MOVE_DOWN = ',',
    MOVE_LEFT = '(',
    MOVE_RIGHT = ')',
    DASH_UP = '^',
    DASH_DOWN = '_',
    DASH_LEFT = '<',
    DASH_RIGHT = '>'
};

enum class Eye
{
    NONE = ' ',
    CIRCLE = '$',
    SQUARE = '%',
    TRIANGLE = '&'
};

struct Transform
{
    Transform(const glm::vec3& position = glm::vec3(1.0), const glm::vec3& scale = glm::vec3(1.0), const glm::quat& rotation = glm::quat())
        : position(position),
          scale(scale),
          rotation(rotation),
          matrix(1.0)
    {}

    const glm::mat4& mat() const
    {
        matrix = glm::mat4(1.0f);

        // Translate
        matrix = glm::translate(matrix, position);

        // Rotate by quaternion
        matrix *= glm::toMat4(rotation);

        // Scale
        matrix = glm::scale(matrix, scale);

        return matrix;
    }

    glm::vec3& pos()
    {
        return position;
    }

    const glm::vec3& pos() const
    {
        return position;
    }

    void setPos(const glm::vec3& newPos)
    {
        position = newPos;
    }

    const glm::vec3& scl() const
    {
        return scale;
    }

    void setScl(const glm::vec3& newScl)
    {
        scale = newScl;
    }

    glm::vec3 rot() const
    {
        return glm::eulerAngles(rotation);
    }

    void setRot(const glm::vec3& newRot)
    {
        rotation = glm::quat(newRot);
    }

protected:
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;

    mutable glm::mat4 matrix;
};

struct Vertex
{
    glm::vec3 pos{};
    glm::vec3 normal{};

    glm::vec2 texCoord{};

    glm::vec3 tangent{};
    glm::vec3 bitangent{};
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
