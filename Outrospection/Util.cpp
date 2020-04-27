#include "Util.h"

#include <sstream>
#include <iostream>

#include "External/stb_image.h"

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
	std::string::const_iterator end = input.end();
	std::string::const_iterator next = std::find(start, end, delimiter);

	while (next != end)
	{
		out.push_back(std::string(start, next));
		start = next + 1;

		next = std::find(start, end, delimiter);
	}

	out.push_back(std::string(start, next));
}

glm::vec3 Util::vecFromYaw(float yawDeg)
{
	glm::vec3 front;
	front.x = cos(glm::radians(yawDeg));
	front.y = 0;
	front.z = sin(glm::radians(yawDeg));
	front = glm::normalize(front);

	return front;
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

const RayHit noHit = RayHit{ -NAN, glm::vec3(0.0) };

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
	if (bothSides ? abs(det) : det < 0.000001) // parallel
		return noHit;

	float invDet = 1.0 / det;

	glm::vec3 tvec = ray.origin - v0;

	float u = dot(tvec, pvec) * invDet;

	if (u < 0 || u > 1)
		return noHit;

	glm::vec3 qvec = cross(tvec, v0v1);

	float v = dot(ray.direction, qvec) * invDet;

	if (v < 0 || u + v > 1)
		return noHit;

	float ret = dot(v0v2, qvec) * invDet;

	if(ret < 0)
		return noHit;

	glm::vec3 hitPos = glm::normalize(ray.direction) * ret;

	return RayHit{ ret, hitPos, tri };
}

// Cast a ray against an infinite plane with the triangle's normal, return where ray hits plane or NaN if ray never hits
glm::vec3 Util::rayCastPlane(const Ray& r, const Triangle& plane) {
	glm::vec3 diff = r.origin - plane.v0;
	float prod1 = glm::dot(diff, -plane.n);
	float prod2 = glm::dot(r.direction, -plane.n);
	float prod3 = prod1 / prod2;
	return r.origin - r.direction * prod3;
}

glm::vec3 Util::getNormal(const Triangle& t) {
	glm::vec3 v0v1 = t.v1 - t.v0;
	glm::vec3 v0v2 = t.v2 - t.v0;
	glm::vec3 normal = glm::cross(v0v1, v0v2);

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
	glm::vec3 bn = b / glm::length(b);
	return bn * glm::dot(a, bn);
}
