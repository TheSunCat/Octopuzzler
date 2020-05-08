#include "Util.h"

#include <sstream>
#include <iostream>

#include <GLAD/glad.h>

#include "External/stb_image.h"
#include "Macros.h"

bool Util::glError(bool print)
{
	bool ret = false;
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		ret = true;

		if (print)
			std::cout << err << std::endl;
	}

	return ret;
}

void Util::split(const std::string& input, const char& delimiter, std::vector<std::string>& out) {
	std::string::const_iterator start = input.begin();
	const std::string::const_iterator end = input.end();
	std::string::const_iterator next = std::find(start, end, delimiter);

	while (next != end)
	{
		out.emplace_back(start, next);
		start = next + 1;

		next = std::find(start, end, delimiter);
	}

	out.emplace_back(start, next);
}

glm::vec3 Util::rotToVec3(float yaw, float pitch)
{
	glm::vec3 ret;
	ret.x = cos(glm::radians(yaw)) * (pitch == 0 ? 1 : cos(glm::radians(pitch)));
	ret.y = pitch == 0 ? 0 : sin(glm::radians(pitch));
	ret.z = sin(glm::radians(yaw)) * (pitch == 0 ? 1 : cos(glm::radians(pitch)));
	return glm::normalize(ret);
}

std::string Util::vecToStr(const glm::vec3& vec)
{
	std::stringstream ss("vec3(");

	ss << vec.x << ", " << vec.y << ", " << vec.z << ")";

	return ss.str();
}

unsigned char* Util::DataFromFile(const char* path, const std::string& directory, int* widthOut, int* heightOut)
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
		std::cout << "Texture data failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	return data;
}

// partly based on https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution

const RayHit NO_HIT = RayHit{ -NAN, glm::vec3(0.0) };

// Cast a ray against finite triangle tri, and return the distance from the ray to the tri.
RayHit Util::rayCast(
	const Ray& ray,
	const Triangle& tri, bool bothSides)
{
	// get vertices from triangle
	glm::vec3 v0 = tri.v0;
	glm::vec3 v1 = tri.v1;
	glm::vec3 v2 = tri.v2;

	glm::vec3 v0v1 = v1 - v0;
	glm::vec3 v0v2 = v2 - v0;

	glm::vec3 pvec = cross(ray.direction, v0v2);

	float det = dot(v0v1, pvec);

	// add abs if tri can be touched from both sides
	if (bothSides ? abs(det) : det < 0.000001f) // parallel
		return NO_HIT;

	float invDet = 1.0f / det;

	glm::vec3 tvec = ray.origin - v0;

	float u = dot(tvec, pvec) * invDet;

	if (u < 0 || u > 1)
		return NO_HIT;

	glm::vec3 qvec = cross(tvec, v0v1);

	float v = dot(ray.direction, qvec) * invDet;

	if (v < 0 || u + v > 1)
		return NO_HIT;

	float ret = dot(v0v2, qvec) * invDet;

	if(ret < 0)
		return NO_HIT;

	glm::vec3 hitPos = glm::normalize(ray.direction) * ret;

	return RayHit{ ret, hitPos, tri };
}

// Cast a ray against an infinite plane with the triangle's normal, return where ray hits plane or NaN if ray never hits
glm::vec3 Util::rayCastPlane(const Ray& r, const Triangle& plane) {
	const glm::vec3 diff = r.origin - plane.v0;
	const float prod1 = glm::dot(diff, -plane.n);
	const float prod2 = glm::dot(r.direction, -plane.n);
	const float prod3 = prod1 / prod2;
	return r.origin - r.direction * prod3;
}

RayHit Util::rayCast(const Ray& r, const std::vector<Triangle>& tris, bool bothSides)
{
	RayHit closestHit = RayHit{ INFINITY };

	for (const Triangle& tri : tris)
	{
		const RayHit hit = Util::rayCast(r, tri, bothSides);

		if (hit.dist < closestHit.dist)
		{
			closestHit = hit;
			closestHit.tri = tri;
		}
	}

	if (closestHit.dist == INFINITY)
		return NO_HIT;
	else
		return closestHit;
}

glm::vec3 Util::getNormal(const Triangle& t) {
	const glm::vec3 v0v1 = t.v1 - t.v0;
	const glm::vec3 v0v2 = t.v2 - t.v0;
	const glm::vec3 normal = glm::cross(v0v1, v0v2);

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
	float angle = glm::dot(a, b);
	angle /= (glm::length(a) * glm::length(b));
	return angle = acosf(angle);
}

glm::vec3 Util::projectV3(const glm::vec3 a, const glm::vec3 b)
{
	const glm::vec3 bn = b / glm::length(b);
	return bn * glm::dot(a, bn);
}

float Util::valFromJoystickAxis(float axis)
{
	const float absxis = std::fabs(axis);
	if (absxis < STICK_DEADZONE)
		axis = 0.0f;
	else if (absxis > STICK_LIMITZONE)
		axis = axis < 0 ? -1.0f : 1.0f;

	return axis;
}
