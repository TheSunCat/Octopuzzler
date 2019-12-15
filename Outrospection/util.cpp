#include "Util.h"

std::vector<std::string> split(std::string input, std::string delimiter) {
	std::vector<std::string>* toReturn = new std::vector<std::string>();
	bool finished = false;
	while (!finished) {
		int location = input.find(delimiter);
		if (location == std::string::npos)
		{
			finished = true;
			toReturn->push_back(input);
		}
		else
		{
			toReturn->push_back(input.substr(0, location));
			input = input.substr(location + delimiter.length());
		}
	}
	return *toReturn;
}

glm::vec3 vecFromYaw(float yawDeg)
{
	glm::vec3 front;
	front.x = cos(glm::radians(yawDeg));
	front.y = 0;
	front.z = sin(glm::radians(yawDeg));
	front = glm::normalize(front);

	return front;
}

unsigned int TextureFromFile(const char* path, const std::string& directory)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int tex;
	int width, height;
	glGenTextures(1, &tex);

	int nrComponents = 0;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		else // TODO error reporting
			return NULL;

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	return tex;
}

unsigned char* DataFromFile(const char* path, const std::string& directory, int* widthOut, int* heightOut)
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
		std::cout << "Texture anim failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	return data;
}

// thanks to https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
float rayCast(
	const Ray& ray,
	const Triangle& tri)
{
	// get vertices from triangle
	glm::vec3 v0 = tri.v0;
	glm::vec3 v1 = tri.v1;
	glm::vec3 v2 = tri.v2;

	// compute plane's normal
	glm::vec3 v0v1 = v1 - v0;
	glm::vec3 v0v2 = v2 - v0;
	// no need to normalize
	glm::vec3 N = glm::cross(v0v1, v0v2); // N

	// get info from ray
	glm::vec3 orig = ray.origin;
	glm::vec3 dir = ray.direction;

	float t; // distance of ray from triangle, if no hit = -1

	// find intersection point with the tri's plane

	// check if ray and plane are parallel, thus avoiding divide by 0
	float NdotDir = glm::dot(N, dir);
	if (fabs(NdotDir) < std::numeric_limits<float>::epsilon()) // epsilon = almost 0 
		return -1; // ray and plane are parallel, return no hit 

	// compute d parameter using equation 2
	float d = glm::dot(N, v0);

	// compute t (equation 3)
	t = -(glm::dot(N, orig) + d) / NdotDir;
	// check if the triangle is behind the ray
	if (t < 0) return t; // the triangle is behind 

	// compute the intersection point using equation 1
	glm::vec3 P = orig + t * dir;

	// Step 2: inside-outside test
	glm::vec3 C; // vector perpendicular to triangle's plane 

	// edge 0
	glm::vec3 edge0 = v1 - v0;
	glm::vec3 vp0 = P - v0;
	C = glm::cross(edge0, vp0);
	if (glm::dot(N, C) < 0) return -1; // P is on the right side 

	// edge 1
	glm::vec3 edge1 = v2 - v1;
	glm::vec3 vp1 = P - v1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(N, C) < 0)  return -1; // P is on the right side 

	// edge 2
	glm::vec3 edge2 = v0 - v2;
	glm::vec3 vp2 = P - v2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(N, C) < 0) return -1; // P is on the right side; 

	return t; // this ray hits the triangle
}