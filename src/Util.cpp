#include "Util.h"
#include "Core.h"

#include <map>
#include <sstream>
#include <fstream>
#ifndef PLATFORM_XP
#include <filesystem>
#else
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <strsafe.h>
#endif

#include <GLAD/glad.h>
#include <glm/common.hpp>
#include <stb_image.h>
#include <fast_float.h>

#include "Types.h"
#include "Constants.h"
#include "Outrospection.h"

glm::vec3 operator*(const int& lhs, const glm::vec3& vec)
{
    return glm::vec3(vec.x * lhs, vec.y * lhs, vec.z * lhs);
}

glm::vec2 operator*(int i, const glm::vec2& vec)
{
    return vec * float(i);
}

glm::vec2 operator*(const glm::vec2& vec, int i)
{
    return i * vec;
}

glm::vec2 operator/(const glm::vec2& left, const glm::ivec2& right)
{
    return left / glm::vec2(right);
}

glm::vec2 operator/(const glm::ivec2& left, const glm::vec2& right)
{
    return glm::vec2(left) / right;
}

glm::vec2 operator*(const glm::vec2& left, const glm::ivec2& right)
{
    return left * glm::vec2(right);
}

glm::vec2 operator*(const glm::ivec2& left, const glm::vec2& right)
{
    return glm::vec2(left) * right;
}

std::string operator+(const std::string& str, int i)
{
    return str + std::to_string(i);
}

std::string operator+(int i, const std::string& str)
{
    return std::to_string(i) + str;
}

SimpleTexture& animatedTexture(const Resource& resource, int tickLength, int frameCount, const GLint& filter)
{
    return Outrospection::get().textureManager.loadAnimatedTexture(resource, tickLength, frameCount, filter);
}

SimpleTexture& simpleTexture(const Resource& resource, const GLint& filter)
{
    return Outrospection::get().textureManager.loadTexture(resource, filter);
}

bool Util::glError()
{
    bool ret = false;
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        ret = true;

        LOG_ERROR("OpenGL error %i", err);
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

time_t Util::currentTimeMillis()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

Util::FutureRun::FutureRun(std::function<void()> _func, time_t _startTime, time_t _waitTime)
    : func(_func), startTime(_startTime), waitTime(_waitTime)
{ }

void Util::doLater(std::function<void()> func, time_t waitTime)
{
    Outrospection::get().futureFunctions.emplace_back(func, currentTimeMillis(), waitTime);
}

bool Util::fileExists(const std::string& file)
{
    std::string fullPath(file);

    LOG("Checking file %s", fullPath);

#ifndef PLATFORM_XP
    return std::filesystem::exists(fullPath);
#else
    return PathFileExistsA(fullPath.c_str());
#endif
}

std::vector<std::string> Util::listFiles(const std::string& dir)
{
    std::vector<std::string> ret;

#ifndef PLATFORM_XP
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        auto str = entry.path().string();
        std::replace(str.begin(), str.end(), '\\', '/');

        ret.emplace_back(str);
    }
#else

    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    TCHAR szDir[MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.
    StringCchCopy(szDir, MAX_PATH, dir.c_str());
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    // Find the first file in the directory.
    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind)
    {
        LOG_ERROR("FindFirstFile");
        return ret;
    }

    do
    {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            ret.emplace_back(ffd.cFileName);
        }
    } while (FindNextFile(hFind, &ffd) != 0);
#endif

    return ret;
}

std::string Util::readAllBytes(const std::string& file)
{
    std::string fullPath(file);
    
    std::ifstream fileStream;

    // ensure ifstream objects can throw exceptions
    fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open file
        fileStream.open(fullPath);
        std::stringstream fileString;

        // read file's buffer contents into streams
        fileString << fileStream.rdbuf();

        // close file handlers
        fileStream.close();

        // convert stream into string
        return fileString.str();
    }
    catch (std::ifstream::failure& e)
    {
        LOG_ERROR("Failed to read file \"%s\"! errno %s", fullPath, e.what());
        return "ERROR: NO FILE";
    }
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

std::string Util::vecToStr(const glm::vec2& vec)
{
    return std::to_string(vec.x) + ", " + std::to_string(vec.y);
}

unsigned char* Util::imageDataFromFile(const char* path, const std::string& directory, int* widthOut, int* heightOut)
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

bool Util::intersectAABB(const AABB& a, const AABB& b)
{
    if (a.min.x < b.min.x)
        return false;
    if (a.min.y < b.min.y)
        return false;
    if (a.min.z < b.min.z)
        return false;
    if (a.max.x < b.max.x)
        return false;
    if (a.max.x < b.max.x)
        return false;
    if (a.max.x < b.max.x)
        return false;

    return true;
}

bool Util::intersectFrustumAABB(const Plane* planes, const AABB& aabb) {

    int result = true; // INSIDE
    for (unsigned int i = 0; i < 6; i++) {
        const auto& plane = planes[i];

        float d = glm::dot(aabb.center, plane.n);
        float r = glm::dot(aabb.halfSize, abs(plane.n));

        if (d + r < -plane.equation[3]) {
            return false; // OUTSIDE
        }

        if (d - r < -plane.equation[3]) {
            result = true; // INTERSECT
        }
    }

    return result;
}

void Util::getFrustumFromViewProj(const glm::mat4& viewProj, Plane* frustum)
{


    // Left Frustum Plane
    // Add first column of the matrix to the fourth column
    frustum[0] = Plane(viewProj[3][0] + viewProj[0][0],
                    viewProj[3][1] + viewProj[0][1],
                    viewProj[3][2] + viewProj[0][2],
                    viewProj[3][3] + viewProj[0][3]);

    // Right Frustum Plane
    // Subtract first column of matrix from the fourth column
    frustum[1] = Plane(viewProj[3][0] - viewProj[0][0],
                    viewProj[3][1] - viewProj[0][1],
                    viewProj[3][2] - viewProj[0][2],
                    viewProj[3][3] - viewProj[0][3]);

    // Top Frustum Plane
    // Subtract second column of matrix from the fourth column
    frustum[2] = Plane(viewProj[3][0] - viewProj[1][0],
                    viewProj[3][1] - viewProj[1][1],
                    viewProj[3][2] - viewProj[1][2],
                    viewProj[3][3] - viewProj[1][3]);

    // Bottom Frustum Plane
    // Add second column of the matrix to the fourth column
    frustum[3] = Plane(viewProj[3][0] + viewProj[1][0],
                    viewProj[3][1] + viewProj[1][1],
                    viewProj[3][2] + viewProj[1][2],
                    viewProj[3][3] + viewProj[1][3]);

    // Near Frustum Plane
    // We could add the third column to the fourth column to get the near plane,
    // but we don't have to do this because the third column IS the near plane
    frustum[4] = Plane(viewProj[2][0],
                    viewProj[2][1],
                    viewProj[2][2],
                    viewProj[2][3]);

    // Far Frustum Plane
    // Subtract third column of matrix from the fourth column
    frustum[5] = Plane(viewProj[3][1] - viewProj[2][0],
                    viewProj[3][1] - viewProj[2][1],
                    viewProj[3][2] - viewProj[2][2],
                    viewProj[3][3] - viewProj[2][3]);
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

float Util::dist2(const glm::vec2& v0, const glm::vec2& v1)
{
    return pow(v0.x - v1.x, 2) + pow(v0.y - v1.y, 2);
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

float Util::major(const glm::vec3& v)
{
    return std::max(std::max(v.x, v.y), v.z);
}

float Util::major(const glm::vec2& v)
{
    return std::max(v.x, v.y);
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
    fast_float::from_chars(str.data(), str.data() + str.size(), ret);
    return ret;
}

int Util::stoi(const std::string_view& str)
{
    return int(Util::stof(str)); // lolrip, fast_float doesn't support int
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


//
// Perlin noise generator
// Keijiro Takahashi, 2013, 2015
// https://github.com/keijiro/PerlinNoise
//
// Based on the original implementation by Ken Perlin
// http://mrl.nyu.edu/~perlin/noise/
//


static const int perm_data[] = {
        151,160,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
        151
    };

const int* Util::Perlin::perm = perm_data;

float Util::Perlin::noise(float x)
{
    int X = int(x) & 0xff;
    x -= floor(x);
    float u = fade(x);
    return Util::lerp(grad(perm[X], x), grad(perm[X + 1], x - 1), u) * 2;
}

float Util::Perlin::noise(float x, float y)
{
    int X = int(x) & 0xff;
    int Y = int(y) & 0xff;
    x -= int(x);
    y -= int(y);
    float u = fade(x);
    float v = fade(y);
    int A = (perm[X] + Y) & 0xff;
    int B = (perm[X + 1] + Y) & 0xff;
    return Util::lerp(Util::lerp(grad(perm[A], x, y), grad(perm[B], x - 1, y), u),
        Util::lerp(grad(perm[A + 1], x, y - 1), grad(perm[B + 1], x - 1, y - 1), u), v);
}

float Util::Perlin::noise(glm::vec2& coord)
{
    return noise(coord.x, coord.y);
}

float Util::Perlin::noise(float x, float y, float z)
{
    int X = int(x) & 0xff;
    int Y = int(y) & 0xff;
    int Z = int(z) & 0xff;
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);
    int A = (perm[X] + Y) & 0xff;
    int B = (perm[X + 1] + Y) & 0xff;
    int AA = (perm[A] + Z) & 0xff;
    int BA = (perm[B] + Z) & 0xff;
    int AB = (perm[A + 1] + Z) & 0xff;
    int BB = (perm[B + 1] + Z) & 0xff;
    return Util::lerp(Util::lerp(Util::lerp(grad(perm[AA], x, y, z), grad(perm[BA], x - 1, y, z), u),
        Util::lerp(grad(perm[AB], x, y - 1, z), grad(perm[BB], x - 1, y - 1, z), u), v),
        Util::lerp(Util::lerp(grad(perm[AA + 1], x, y, z - 1), grad(perm[BA + 1], x - 1, y, z - 1), u),
            Util::lerp(grad(perm[AB + 1], x, y - 1, z - 1), grad(perm[BB + 1], x - 1, y - 1, z - 1), u), v), w);
}

float Util::Perlin::noise(const glm::vec3& coord)
{
    return noise(coord.x, coord.y, coord.z);
}

float Util::Perlin::fbm(float x, int octave)
{
    float f = 0.0f;
    float w = 0.5f;
    for (float i = 0; i < octave; i++) {
        f += w * noise(x);
        x *= 2.0f;
        w *= 0.5f;
    }
    return f;
}

float Util::Perlin::fbm(glm::vec2& coord, int octave)
{
    return fbm(coord.x, coord.y, octave);
}

float Util::Perlin::fbm(float x, float y, int octave)
{
    float f = 0.0f;
    float w = 0.5f;
    for (int i = 0; i < octave; i++) {
        f += w * noise(x, y);
        x *= 2.0f; y *= 2.0f;
        w *= 0.5f;
    }
    return f;
}

float Util::Perlin::fbm(glm::vec3& coord, int octave)
{
    return fbm(coord.x, coord.y, coord.z, octave);
}

float Util::Perlin::fbm(float x, float y, float z, int octave)
{
    float f = 0.0f;
    float w = 0.5f;
    for (int i = 0; i < octave; i++) {
        f += w * noise(x, y, z);
        x *= 2.0f; y *= 2.0f; z *= 2.0f;
        w *= 0.5f;
    }
    return f;
}

float Util::Perlin::fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float Util::Perlin::grad(int hash, float x)
{
    return (hash & 1) == 0 ? x : -x;
}

float Util::Perlin::grad(int hash, float x, float y)
{
    return ((hash & 1) == 0 ? x : -x) + ((hash & 2) == 0 ? y : -y);
}

float Util::Perlin::grad(int hash, float x, float y, float z)
{
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}