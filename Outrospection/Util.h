#pragma once
#include "Core.h"

#include <glm/glm.hpp>

#include "Types.h"

glm::vec3 operator*(const int& lhs, const glm::vec3& vec);

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

    std::size_t hashBytes(const char* data, std::size_t length);

    glm::vec3 rotToVec3(float yaw, float pitch = 0);

    std::string vecToStr(const glm::vec3& vec);

    unsigned char* dataFromFile(const char* path, const std::string& directory, int* widthOut, int* heightOut);

    Collision rayCast(
        const Ray& ray,
        const Triangle& tri, bool bothSides);

    glm::vec3 rayCastPlane(const Ray& r, const Triangle& plane);

    Collision rayCast(const Ray& r, const std::vector<Triangle>& tris, bool bothSides = false);

    Collision rayCast(const Ray& r, const std::vector<std::vector<Triangle>::const_iterator>& tris,
                      bool bothSides = false);

    bool intersectRaySegmentSphere(const Ray& ray, glm::vec3 sphereOrigin, float sphereRadius2,
                                   glm::vec3& intersectPoint);

    bool inTriangle(const glm::vec3& point, const Triangle& tri);

    bool intersectTriangleSphere(const glm::vec3& spherePos, float sphereRadius, const Triangle& tri, glm::vec3& intersectPoint, float& pointToPlaneDist);

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

    float length2(const glm::vec3& v);

    bool isZeroV3(const glm::vec3& v);

    float sumAbsV3(const glm::vec3& v);

    float angleBetweenV3(glm::vec3 a, glm::vec3 b);

    float cosBetweenV3(glm::vec3 a, glm::vec3 b);

    glm::vec3 projectV3(glm::vec3 a, glm::vec3 b);

    bool lowestRoot(float a, float b, float c, float maxRoot, float& root);

    // return input depending on constant deadzones and limitzones
    float valFromJoystickAxis(float axis);

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
}
