#pragma once
#include "Core.h"

#include <unordered_map>
//#include <unordered_set>

#include <GLAD/glad.h>

#include "Resource.h"
#include "Types.h"

#include "Core/Rendering/SimpleTexture.h"

class TextureManager
{
private:
    std::unordered_map<Resource, std::unique_ptr<SimpleTexture>, Hashes> textures;
public:
    TextureManager();

    SimpleTexture& loadTexture(const Resource& r);

    SimpleTexture& loadAnimatedTexture(const Resource& r, unsigned int textureTickLength,
                                        unsigned int textureFrameCount);

    void bindTexture(Resource& r);

    SimpleTexture& get(const Resource& r);

    void tickAllTextures();

    static SimpleTexture missingTexture;

    static unsigned char* readImageBytes(const std::string& path, int& width, int& height);
    static void free(unsigned char* data);
	
    DISALLOW_COPY_AND_ASSIGN(TextureManager);
private:
    static GLuint textureFromFile(const std::string& filename);
    static void createTexture(const GLuint& texId, const unsigned char* data, const GLenum& format,
                              const unsigned int& width, const unsigned int& height);
};
