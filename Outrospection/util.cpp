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

	glm::vec3 v0v1 = v1 - v0;
	glm::vec3 v0v2 = v2 - v0;

	glm::vec3 pvec = cross(ray.direction, v0v2);

	float det = dot(v0v1, pvec);

	if (abs(det) < 0.000001)
		return -INFINITY;

	float invDet = 1.0 / det;

	glm::vec3 tvec = ray.origin - v0;

	float u = dot(tvec, pvec) * invDet;

	if (u < 0 || u > 1)
		return -INFINITY;

	glm::vec3 qvec = cross(tvec, v0v1);

	float v = dot(ray.direction, qvec) * invDet;

	if (v < 0 || u + v > 1)
		return -INFINITY;

	float ret = dot(v0v2, qvec) * invDet;

	if(ret < 0)
		return -INFINITY;

	return ret;
}