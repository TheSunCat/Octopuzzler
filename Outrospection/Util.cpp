#include "Util.h"
#include "Core.h"

#include <charconv>
#include <sstream>

#include <GLAD/glad.h>
#include <glm/common.hpp>

#include "Constants.h"
#include "External/stb_image.h"

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

void Util::split(const std::string& input, const char& delimiter, std::vector<std::string_view>& out, const int startCut, const int endCut) {
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

    uint32_t hash = length;

    if (length <= 0 || data == nullptr) return 0;

    int rem = length & 3;
    length >>= 2;

    /* Main loop */
    for (; length > 0; length--) {
        hash += get16bits(data);
        uint32_t tmp = (get16bits(data + 2) << 11) ^ hash;
        hash = (hash << 16) ^ tmp;
        data += 2 * sizeof(uint16_t);
        hash += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
    case 3: hash += get16bits(data);
        hash ^= hash << 16;
        hash ^= ((signed char)data[sizeof(uint16_t)]) << 18;
        hash += hash >> 11;
        break;
    case 2: hash += get16bits(data);
        hash ^= hash << 11;
        hash += hash >> 17;
        break;
    case 1: hash += (signed char) * data;
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

const Collision NO_HIT = Collision{ INFINITY, glm::vec3(0.0) };

// cast a ray against finite tri, ret distance from the ray to the tri
Collision Util::rayCast(
    const Ray& ray,
    const Triangle& tri, const bool bothSides)
{
    const glm::vec3 v0v1 = tri.v1 - tri.v0;
    const glm::vec3 v0v2 = tri.v2 - tri.v0;

    const glm::vec3 pvec = cross(ray.direction, v0v2);

    float det = dot(v0v1, pvec);

    // add abs if tri can be touched from both sides
    if ((bothSides ? abs(det) : det) < 0.000001f) // parallel
        return NO_HIT;

    float invDet = 1.0f / det;

    const glm::vec3 tvec = ray.origin - tri.v0;

    float u = dot(tvec, pvec) * invDet;

    if (u < 0 || u > 1)
        return NO_HIT;

    const glm::vec3 qvec = cross(tvec, v0v1);

    float v = dot(ray.direction, qvec) * invDet;

    if (v < 0 || u + v > 1)
        return NO_HIT;

    float ret = dot(v0v2, qvec) * invDet;

    if(ret < 0)
        return NO_HIT;

    return Collision { ret, tri };
}

// Cast a ray against an infinite plane with the triangle's normal, return where ray hits plane or NaN if ray never hits
glm::vec3 Util::rayCastPlane(const Ray& r, const Triangle& plane) {
    const glm::vec3 diff = r.origin - plane.v0;
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
bool Util::intersectRaySegmentSphere(const Ray& ray, const glm::vec3 sphereOrigin, const float sphereRadius2, glm::vec3& intersectPoint)
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
    const bool withinTriPrism = sameSide(point, tri.v0, tri.v1, tri.v2) && sameSide(point, tri.v1, tri.v0, tri.v2)
        && sameSide(point, tri.v2, tri.v0, tri.v1);

    // if it isn't it will never be on the triangle
    if (!withinTriPrism)
        return false;

    // calc normal
    const glm::vec3 n = genNormal(tri);

    // project the point onto normal
    const glm::vec3 proj = Util::projectV3(point, n);

    // if the distance from the triangle to the point is 0
    if (Util::length2V3(proj) == 0)
        return true;
    else
        return false;
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

glm::vec3 Util::genNormal(const Triangle& t) {
    const glm::vec3 u = t.v1 - t.v0;
    const glm::vec3 v = t.v2 - t.v0;
    const glm::vec3 normal = glm::cross(u, v);

    return normalize(normal);
}

float Util::length2V3(const glm::vec3& v)
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

float Util::valFromJoystickAxis(float axis)
{
    const float absxis = std::fabs(axis); // absolute axis. absxis.
    if (absxis < STICK_DEADZONE)
        axis = 0.0f;
    else if (absxis > STICK_LIMITZONE)
        axis = axis < 0 ? -1.0f : 1.0f;

    return axis;
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