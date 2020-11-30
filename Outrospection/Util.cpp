#include "Util.h"
#include "Core.h"

#include <charconv>
#include <map>
#include <sstream>

#include <GLAD/glad.h>
#include <glm/common.hpp>

#include "Constants.h"
#include "External/stb_image.h"

glm::vec3 operator*(const int& lhs, const glm::vec3& vec)
{
    return glm::vec3(vec.x * lhs, vec.y * lhs, vec.z * lhs);
}

bool Util::glError()
{
    bool ret = false;
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        ret = true;

        LOG_ERROR("%i", err);
    }

    return ret;
}

void Util::split(const std::string& input, const char& delimiter, std::vector<std::string_view>& out,
                 const int startCut, const int endCut)
{
    auto start = input.begin() + startCut;
    const auto end = input.end() - endCut;
    auto next = std::find(start, end, delimiter);

    while (next != end)
    {
        out.emplace_back(&*start, next - start);
        start = next + 1;

        next = std::find(start, end, delimiter);
    }

    out.emplace_back(&*start, next - start);
}

std::size_t Util::hashBytes(const char* data, std::size_t length)
{
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8) +(uint32_t)(((const uint8_t *)(d))[0]))

    auto hash = uint32_t(length);

    if (length <= 0 || data == nullptr) return 0;

    int rem = length & 3;
    length >>= 2;

    /* Main loop */
    for (; length > 0; length--)
    {
        hash += get16bits(data);
        uint32_t tmp = (get16bits(data + 2) << 11) ^ hash;
        hash = (hash << 16) ^ tmp;
        data += 2 * sizeof(uint16_t);
        hash += hash >> 11;
    }

    /* Handle end cases */
    switch (rem)
    {
    case 3: hash += get16bits(data);
        hash ^= hash << 16;
        hash ^= ((signed char)data[sizeof(uint16_t)]) << 18;
        hash += hash >> 11;
        break;
    case 2: hash += get16bits(data);
        hash ^= hash << 11;
        hash += hash >> 17;
        break;
    case 1: hash += (signed char)*data;
        hash ^= hash << 10;
        hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

glm::vec3 Util::rotToVec3(const float yaw, const float pitch)
{
    glm::vec3 ret;
    ret.x = cos(glm::radians(yaw)) * (pitch == 0 ? 1 : cos(glm::radians(pitch)));
    ret.y = pitch == 0 ? 0 : sin(glm::radians(pitch));
    ret.z = sin(glm::radians(yaw)) * (pitch == 0 ? 1 : cos(glm::radians(pitch)));
    return glm::normalize(ret);
}

std::string Util::vecToStr(const glm::vec3& vec)
{
    return std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z);
}

unsigned char* Util::dataFromFile(const char* path, const std::string& directory, int* widthOut, int* heightOut)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    int nrComponents = 0;
    unsigned char* data = stbi_load(filename.c_str(), widthOut, heightOut, &nrComponents, 0);
    if (data)
    {
        stbi_image_free(data);
    }
    else
    {
        LOG_ERROR("Texture data failed to load at path: %s", filename);
        stbi_image_free(data);
    }

    return data;
}

// partly based on https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution

const Collision NO_HIT = Collision{INFINITY, glm::vec3(0.0)};

// cast a ray against finite tri, ret distance from the ray to the tri
Collision Util::rayCast(
    const Ray& ray,
    const Triangle& tri, const bool bothSides)
{
    const glm::vec3 v0v1 = tri.verts[1] - tri.verts[0];
    const glm::vec3 v0v2 = tri.verts[2] - tri.verts[0];

    const glm::vec3 pvec = cross(ray.direction, v0v2);

    float det = dot(v0v1, pvec);

    // add abs if tri can be touched from both sides
    if ((bothSides ? abs(det) : det) < 0.000001f) // parallel
        return NO_HIT;

    float invDet = 1.0f / det;

    const glm::vec3 tvec = ray.origin - tri.verts[0];

    float u = dot(tvec, pvec) * invDet;

    if (u < 0 || u > 1)
        return NO_HIT;

    const glm::vec3 qvec = cross(tvec, v0v1);

    float v = dot(ray.direction, qvec) * invDet;

    if (v < 0 || u + v > 1)
        return NO_HIT;

    float ret = dot(v0v2, qvec) * invDet;

    if (ret < 0)
        return NO_HIT;

    return Collision{ret, tri};
}

// Cast a ray against an infinite plane with the triangle's normal, return where ray hits plane or NaN if ray never hits
glm::vec3 Util::rayCastPlane(const Ray& r, const Triangle& plane)
{
    const glm::vec3 diff = r.origin - plane.verts[0];
    const float prod1 = glm::dot(diff, -plane.n);
    const float prod2 = glm::dot(r.direction, -plane.n);
    const float prod3 = prod1 / prod2;
    return r.origin - r.direction * prod3;
}

Collision Util::rayCast(const Ray& r, const std::vector<Triangle>& tris, bool bothSides)
{
    auto closestHit = NO_HIT;

    for (const Triangle& tri : tris)
    {
        const Collision hit = Util::rayCast(r, tri, bothSides);

        if (hit.dist < closestHit.dist)
        {
            closestHit = hit;
            closestHit.tri = tri;
        }
    }

    return closestHit;
}

Collision Util::rayCast(const Ray& r, const std::vector<std::vector<Triangle>::const_iterator>& tris, bool bothSides)
{
    auto closestHit = NO_HIT;

    for (const auto& tri : tris)
    {
        const Collision hit = Util::rayCast(r, *tri, bothSides);

        if (hit.dist < closestHit.dist)
        {
            closestHit = hit;
            closestHit.tri = *tri;
        }
    }

    return closestHit;
}

//https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code
bool Util::intersectRaySegmentSphere(const Ray& ray, const glm::vec3 sphereOrigin, const float sphereRadius2,
                                     glm::vec3& intersectPoint)
{
    const glm::vec3 origin = ray.origin;
    glm::vec3 direction = ray.direction;

    // manual normalization so we know the length
    const float rayLength = glm::length(direction);
    direction /= rayLength;

    const glm::vec3 originDiff = origin - sphereOrigin;
    const float oDiffDotDir = glm::dot(originDiff, direction);
    const float oDiffDotODiffSphereRadius2 = glm::dot(originDiff, originDiff) - sphereRadius2;

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if (oDiffDotODiffSphereRadius2 > 0.0f && oDiffDotDir > 0.0f)
        return false;
    const float discr = oDiffDotDir * oDiffDotDir - oDiffDotODiffSphereRadius2;

    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.0f)
        return false;

    // Ray now found to intersect sphere, compute smallest t value of intersection
    float t = -oDiffDotDir - sqrtf(discr);

    // If t is negative, ray started inside sphere so clamp t to zero
    if (t < 0.0f)
        t = 0.0f;
    intersectPoint = origin + (direction * t);

    //here's that last segment check I was talking about
    if (t > rayLength)
        return false;

    return true;
}

// returns whether a glm::vec3 is within a Triangle
bool Util::inTriangle(const glm::vec3& point, const Triangle& tri)
{
    // test to see if it is within an infinite prism that the triangle outlines
    const bool withinTriPrism = sameSide(point, tri.verts[0], tri.verts[1], tri.verts[2]) && sameSide(point, tri.verts[1], tri.verts[0], tri.verts[2])
        && sameSide(point, tri.verts[2], tri.verts[0], tri.verts[1]);

    // if it isn't it will never be on the triangle
    if (!withinTriPrism)
        return false;

    // calc normal
    const glm::vec3 n = genNormal(tri);

    // project the point onto normal
    const glm::vec3 proj = Util::projectV3(point, n);

    // if the distance from the triangle to the point is 0
    if (Util::length2(proj) == 0)
        return true;
    else
        return false;
}

bool Util::intersectTriangleSphere(const glm::vec3& spherePos, float sphereRadius, const Triangle& tri, glm::vec3& intersectPoint, float& pointToPlaneDist)
{
    glm::vec3 v0 = tri.verts[0];
    glm::vec3 v1 = tri.verts[1];
    glm::vec3 v2 = tri.verts[2];

    v0 = v0 - spherePos;
    v1 = v1 - spherePos;
    v2 = v2 - spherePos;
    float sphereRadiusSquare = sphereRadius * sphereRadius;

    // Check if sphere collides with infinite triangle plane

    float d = abs(glm::dot(v0, tri.n));
    bool outsideOfPlane = d > sphereRadius;

    if (outsideOfPlane)
        return false;

    pointToPlaneDist = d;


    // Test if sphere is outside of each vert of the triangle

    float v0dotv0 = glm::dot(v0, v0);
    float v0dotv1 = glm::dot(v0, v1);
    float v0dotv2 = glm::dot(v0, v2);
    float v1dotv1 = glm::dot(v1, v1);
    float v1dotv2 = glm::dot(v1, v2);
    float v2dotv2 = glm::dot(v2, v2);


    bool outOfVert0 = (v0dotv0 > sphereRadiusSquare) && (v0dotv1 > v0dotv0) && (v0dotv2 > v0dotv0);
    bool outOfVert1 = (v1dotv1 > sphereRadiusSquare) && (v0dotv1 > v1dotv1) && (v1dotv2 > v1dotv1);
    bool outOfVert2 = (v2dotv2 > sphereRadiusSquare) && (v0dotv2 > v2dotv2) && (v1dotv2 > v2dotv2);

    bool outsideOfVerts = outOfVert0 && outOfVert1 && outOfVert2;

    if (outsideOfVerts)
        return false;


    // build 3 rays (line segments) so we can do plane projection later
    glm::vec3 v1v0 = v1 - v0;
    glm::vec3 v2v1 = v2 - v1;
    glm::vec3 v0v2 = v0 - v2;

    // TODO fix this
    //float d1 = v0dotv1 - v0dotv0;
    //float d2 = v1dotv2 - v1dotv1;
    //float d3 = v0dotv2 - v2dotv2;

    //float e1 = glm::dot(v1v0, v1v0);
    //float e2 = glm::dot(v2v1, v2v1);
    //float e3 = glm::dot(v0v2, v0v2);
    //glm::vec3 Q1 = v0 * e1 - d1 * v1v0;
    //glm::vec3 Q2 = v1 * e2 - d2 * v2v1;
    //glm::vec3 Q3 = v2 * e3 - d3 * v0v2;
    //glm::vec3 QC = v2 * e1 - Q1;
    //glm::vec3 QA = v0 * e2 - Q2;
    //glm::vec3 QB = v1 * e3 - Q3;

    //bool outOfEdge0 = (glm::dot(Q1, Q1) > sphereRadiusSquare * e1 * e1) && (glm::dot(Q1, QC) > 0);
    //bool outOfEdge1 = (glm::dot(Q2, Q2) > sphereRadiusSquare * e2 * e2) && (glm::dot(Q2, QA) > 0);
    //bool outOfEdge2 = (glm::dot(Q3, Q3) > sphereRadiusSquare * e3 * e3) && (glm::dot(Q3, QB) > 0);

    bool outOfEdge0 = Util::intersectRaySegmentSphere(Ray{ v0, v1v0 }, spherePos, sphereRadiusSquare, intersectPoint);
    bool outOfEdge1 = Util::intersectRaySegmentSphere(Ray{ v1, v2v1 }, spherePos, sphereRadiusSquare, intersectPoint);
    bool outOfEdge2 = Util::intersectRaySegmentSphere(Ray{ v2, v0v2 }, spherePos, sphereRadiusSquare, intersectPoint);

    bool outsideOfEdges = outOfEdge0 && outOfEdge1 && outOfEdge2;

    bool separated = outsideOfPlane || outsideOfVerts || outsideOfEdges;

    return !separated; // together ♥
}

bool Util::sameSide(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& a, const glm::vec3& b)
{
    const glm::vec3 cp1 = glm::cross(b - a, p1 - a);
    const glm::vec3 cp2 = glm::cross(b - a, p2 - a);

    if (glm::dot(cp1, cp2) >= 0)
        return true;
    else
        return false;
}


bool Util::leftOf(const glm::vec2& a, const glm::vec2& b, const glm::vec2& p)
{
    //3x3 determinant (can also think of this as projecting onto 2D lines)
    // | ax  bx  px |
    // | ay  by  py |
    // | 1   1   1  |

    const float area = 0.5f * (a.x * (b.y - p.y) +
        b.x * (p.y - a.y) +
        p.x * (a.y - b.y));
    return (area > 0.0f);
}

// 2D test for point inside polygon
bool Util::pointInside(const glm::vec2 poly[], const int pcount, const glm::vec2& v)
{
    for (int i = 0; i < pcount; i++)
    {
        int next = i;
        next++;
        if (next == pcount)
            next = 0;

        if (!leftOf(poly[i], poly[next], v))
            return false;
    }
    return true;
}

bool Util::pointInTriangle(const glm::vec3& point, const Triangle& tri)
{
    glm::vec3 e10 = tri.verts[1] - tri.verts[0];
    glm::vec3 e20 = tri.verts[2] - tri.verts[0];

    float a = glm::dot(e10, e10);
    float b = glm::dot(e10, e20);
    float c = glm::dot(e20, e20);

    float ac_bb = (a * c) - (b * b);

    glm::vec3 vp(point.x - tri.verts[0].x, point.y - tri.verts[0].y, point.z - tri.verts[2].z);

    float d = glm::dot(vp, e10);
    float e = glm::dot(vp, e20);

    float x = (d * c) - (e * b);
    float y = (e * a) - (d * b);
    float z = x + y - ac_bb;


    return ((uint32_t(z) & ~(uint32_t(x) | uint32_t(y))) & 0x80000000);
}

#include <array>

GLuint vertex_for_edge(std::map<std::pair<GLuint, GLuint>, GLuint>& lookup, std::vector<glm::vec3>& vertices, GLuint first, GLuint second)
{
    std::map<std::pair<GLuint, GLuint>, GLuint>::key_type key(first, second);
    if (key.first > key.second)
        std::swap(key.first, key.second);

    auto [fst, snd] = lookup.insert({ key, vertices.size() });
    if (snd)
    {
        auto& edge0 = vertices[first];
        auto& edge1 = vertices[second];
        auto point = glm::normalize(edge0 + edge1);
        vertices.push_back(point);
    }

    return fst->second;
}

std::vector<GLuint> Util::subdivide(std::vector<glm::vec3>& vertices,
                                      const std::vector<GLuint>& triangles)
{
    std::map<std::pair<GLuint, GLuint>, GLuint> lookup;
    std::vector<GLuint> result;

    for (auto&& each : triangles)
    {
        std::array<GLuint, 3> mid{};
        for (int edge = 0; edge < 3; ++edge)
        {
            mid[edge] = vertex_for_edge(lookup, vertices, vertices[each][edge], vertices[each][(edge + 1) % 3]);
        }

        vertices.emplace_back(vertices[each][0], mid[0], mid[2]);
        result.emplace_back(vertices.size() - 1);

        vertices.emplace_back(vertices[each][1], mid[1], mid[0]);
        result.emplace_back(vertices.size() - 1);

        vertices.emplace_back(vertices[each][2], mid[2], mid[1]);
        result.emplace_back(vertices.size() - 1);

        vertices.emplace_back(mid[0], mid[1], mid[2]);
        result.emplace_back(vertices.size() - 1);
    }

    return result;
}

glm::vec3 Util::genNormal(const Triangle& t)
{
    const glm::vec3 u = t.verts[1] - t.verts[0];
    const glm::vec3 v = t.verts[2] - t.verts[0];
    const glm::vec3 normal = glm::cross(u, v);

    return glm::normalize(normal);
}

float Util::dist2(const glm::vec3& v0, const glm::vec3& v1)
{
    return pow(v0.x - v1.x, 2) + pow(v0.y - v1.y, 2) + pow(v0.z - v1.z, 2);
}

float Util::length2(const glm::vec3& v)
{
    return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

bool Util::isZeroV3(const glm::vec3& v)
{
    return (v.x == 0 && v.y == 0 && v.z == 0);
}

float Util::sumAbsV3(const glm::vec3& v)
{
    return abs(v.x) + abs(v.y) + abs(v.z);
}

float Util::angleBetweenV3(const glm::vec3 a, const glm::vec3 b)
{
    float angle = glm::dot(a, b);      // dot = length(a) * length(b) * cos(angle)
    angle /= (glm::length(a) * glm::length(b));
    return acosf(angle);
}

float Util::cosBetweenV3(const glm::vec3 a, const glm::vec3 b)
{
    float cos = glm::dot(a, b);
    cos /= (glm::length(a) * glm::length(b));
    return cos;
}

glm::vec3 Util::projectV3(const glm::vec3 a, const glm::vec3 b)
{
    const glm::vec3 bn = b / glm::length(b);
    return bn * glm::dot(a, bn);
}

void Util::setLength(glm::vec3& v, float length)
{
    v = glm::normalize(v) * length;
}

bool Util::lowestRoot(float a, float b, float c, float maxRoot, float& root)
{
    float determinant = b * b - (4.0f * a * c); // b² - 4ac

    if (determinant < 0.0f) // negative sqrt = no solutions
        return false;

    float sqrtD = sqrt(determinant);
    float r0 = (-b - sqrtD) / (2 * a);
    float r1 = (-b + sqrtD) / (2 * a);

    if(r0 > r1)
        Util::swap(r0, r1);

    // get lowest root
    if(r0 > 0 && r0 < maxRoot)
    {
        root = r0;
        return true;
    }

    // if r0 < 0, we want r1
    if(r1 > 0 && r1 < maxRoot)
    {
        root = r1;
        return true;
    }

    // no valid solutions
    return false;
}

float Util::valFromJoystickAxis(float axis)
{
    const float absxis = std::fabs(axis); // absolute axis. absxis.
    if (absxis < STICK_DEADZONE)
        axis = 0.0f;
    else if (absxis > STICK_LIMITZONE)
        axis = axis < 0 ? -1.0f : 1.0f;

    return axis;
}

bool Util::isAllDigits(const std::string_view& str, bool allowDecimals)
{
    const char* allowedChars = allowDecimals ? "0123456789,. " : "0123456789 ";

    return str.find_first_not_of(allowedChars) == std::string::npos;
}

float Util::stof(const std::string_view& str)
{
    float ret;
    std::from_chars(str.data(), str.data() + str.size(), ret);
    return ret;
}

int Util::stoi(const std::string_view& str)
{
    int ret;
    std::from_chars(str.data(), str.data() + str.size(), ret);
    return ret;
}

Util::Timer::Timer() : Timer::Timer("")     { }

Util::Timer::Timer(const char* _name) : begin(std::chrono::high_resolution_clock::now()),
    name(_name) {}

Util::Timer::~Timer()
{
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    LOG_DEBUG("%s took %lld musec", name, dur.count());
}
