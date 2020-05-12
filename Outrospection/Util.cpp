#include "Util.h"

#include <sstream>
#include <iostream>

#include <GLAD/glad.h>
#include <glm/common.hpp>

#include "External/stb_image.h"
#include "Constants.h"

bool Util::glError(const bool print)
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
	std::stringstream ss("vec3(");

	ss << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";

	return ss.str();
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
		std::cout << "Texture data failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	return data;
}

// partly based on https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution

const Collision NO_HIT = Collision{ INFINITY, glm::vec3(0.0) };

// Cast a ray against finite triangle tri, and return the distance from the ray to the tri.
Collision Util::rayCast(
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
	if ((bothSides ? abs(det) : det) < 0.000001f) // parallel
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

	return Collision { ret, hitPos, tri };
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
	Collision closestHit = Collision{ INFINITY };

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

//https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code
bool Util::intersectRaySegmentSphere(const Ray& ray, const glm::vec3 sphereOrigin, const float sphereRadius2, glm::vec3& ip)
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
	ip = origin + (direction * t);

	//here's that last segment check I was talking about
	if (t > rayLength)
		return false;

	return true;
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

//2D test for point inside polygon
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
	const float absxis = std::fabs(axis);
	if (absxis < STICK_DEADZONE)
		axis = 0.0f;
	else if (absxis > STICK_LIMITZONE)
		axis = axis < 0 ? -1.0f : 1.0f;

	return axis;
}
