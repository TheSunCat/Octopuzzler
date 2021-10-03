#pragma once
#include "Core.h"

#include <glm/glm.hpp>

#include "Types.h"

glm::vec3 operator*(const int& lhs, const glm::vec3& vec);
glm::vec2 operator*(int i, const glm::vec2& vec);
glm::vec2 operator*(const glm::vec2& vec, int i);

glm::vec2 operator/(const glm::vec2& left, const glm::ivec2& right);
glm::vec2 operator/(const glm::ivec2& left, const glm::vec2& right);

glm::vec2 operator*(const glm::vec2& left, const glm::ivec2& right);
glm::vec2 operator*(const glm::ivec2& left, const glm::vec2& right);

std::string operator+(const std::string& str, int i);
std::string operator+(int i, const std::string& str);

// proxy functions that are shorter than the usual huge call
SimpleTexture& animatedTexture(const Resource& resource, int tickLength, int frameCount, const GLint& filter);
SimpleTexture& simpleTexture(const Resource& resource, const GLint& filter);

namespace Util
{
    // Check for OpenGL errors and print them
    bool glError();

    void split(const std::string& input, const char& delimiter, std::vector<std::string_view>& out, int startCut = 0,
               int endCut = 0);
    void split(const std::string&& input, const char& delimiter, std::vector<std::string_view>& out, int startCut = 0,
               int endCut = 0) = delete;
    void split(const std::string&& input, const char& delimiter, std::vector<std::string_view>& out,
               int startCut = 0) = delete;
    void split(const std::string&& input, const char& delimiter, std::vector<std::string_view>& out) = delete;

    template <typename T>
    void push_all(std::vector<T>& input, std::vector<T>& add)
    {
        for (const T& o : add)
            input.emplace_back(o);
    }

    template <typename T>
    void swap(T& a, T& b)
    {
        T temp = a;
        b = a;
        a = temp;
    }

    template <typename T>
    T clamp(const T& val, const T& min, const T& max)
    {
        if (min > max)
        {
            LOG_ERROR("min is greater than max! min = %f, max = %f", min, max);
            return val;
        }

        if (val < min)
            return min;
        if (val > max)
            return max;

        return val;
    }

    template <typename T>
    T lerp(const T& startVal, const T& endVal, const float percent)
    {
        const T& difference = endVal - startVal;

        return startVal + difference * percent;
    }

    // I miss Java
    time_t currentTimeMillis();

	// future stuff
    struct FutureRun
    {
        FutureRun(std::function<void()> _func, time_t _startTime, time_t _waitTime);

        std::function<void()> func;
        time_t startTime;

        time_t waitTime;
    };

    void doLater(std::function<void()> func, time_t waitTime);
    
    constexpr std::size_t hashBytes(const char* data, std::size_t length)
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

    bool fileExists(const std::string& file);
    std::vector<std::string> listFiles(const std::string& dir);
    std::string readAllBytes(const std::string& file);

    glm::vec3 rotToVec3(float yaw, float pitch = 0);

    std::string vecToStr(const glm::vec3& vec);
    std::string vecToStr(const glm::vec2& vec);

    unsigned char* imageDataFromFile(const char* path, const std::string& directory, int* widthOut, int* heightOut);

    Collision rayCast(
        const Ray& ray,
        const Triangle& tri, bool bothSides);

    glm::vec3 rayCastPlane(const Ray& r, const Triangle& plane);

    Collision rayCast(const Ray& r, const std::vector<Triangle>& tris, bool bothSides = false);

    Collision rayCast(const Ray& r, const std::vector<std::vector<Triangle>::const_iterator>& tris,
                      bool bothSides = false);

    bool intersectRaySegmentSphere(const Ray& ray, glm::vec3 sphereOrigin, float sphereRadius2,
                                   glm::vec3& intersectPoint);

    bool intersectAABB(const AABB& a, const AABB& b);

    bool intersectFrustumAABB(const Plane* planes, const AABB& aabb);

    void getFrustumFromViewProj(const glm::mat4& viewProj, Plane* frustum);

    bool inTriangle(const glm::vec3& point, const Triangle& tri);

    // A test to see if P1 is on the same side as P2 of a line segment ab
    bool sameSide(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& a, const glm::vec3& b);

    // 2D test for which side of a 2D line a 2D point lies on
    bool leftOf(const glm::vec2& a, const glm::vec2& b, const glm::vec2& p);

    bool pointInside(const glm::vec2 poly[], int pcount, const glm::vec2& v);

    bool pointInTriangle(const glm::vec3& point, const Triangle& tri);

    std::vector<GLuint> subdivide(std::vector<glm::vec3>& vertices,
                                    const std::vector<GLuint>& triangles);

    glm::vec3 genNormal(const Triangle& t);

    float dist2(const glm::vec3& v0, const glm::vec3& v1);

	float dist2(const glm::vec2& v0, const glm::vec2& v1);

    float length2(const glm::vec3& v);

    bool isZeroV3(const glm::vec3& v);

    float sumAbsV3(const glm::vec3& v);

    float major(const glm::vec3& v);
    float major(const glm::vec2& v);

    float angleBetweenV3(glm::vec3 a, glm::vec3 b);

    float cosBetweenV3(glm::vec3 a, glm::vec3 b);

    glm::vec3 projectV3(glm::vec3 a, glm::vec3 b);

    void setLength(glm::vec3& v, float length);

    bool lowestRoot(float a, float b, float c, float maxRoot, float& root);

    // return input depending on constant deadzones and limitzones
    float valFromJoystickAxis(float axis);

    bool isAllDigits(const std::string_view& str, bool allowDecimals = false);

    float stof(const std::string_view& str);

    int stoi(const std::string_view& str);

    class Timer
    {
    public:
        Timer();
        Timer(const char* _name);
        ~Timer();
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> begin;
        const char* name;
    };

    class Perlin
    {
    public:
        static float noise(float x);

        static float noise(float x, float y);
        static float noise(glm::vec2& coord);

        static float noise(float x, float y, float z);
        static float noise(const glm::vec3& coord);

        static float fbm(float x, int octave);

        static float fbm(glm::vec2& coord, int octave);
        static float fbm(float x, float y, int octave);

        static float fbm(glm::vec3& coord, int octave);
        static float fbm(float x, float y, float z, int octave);

        static float fade(float t);

        static float grad(int hash, float x);
        static float grad(int hash, float x, float y);
        static float grad(int hash, float x, float y, float z);
    private:
        static const int* perm;
    };
}
