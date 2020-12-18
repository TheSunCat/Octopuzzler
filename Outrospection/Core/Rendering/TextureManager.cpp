#include "TextureManager.h"

#include <sstream>

#include "External/stb_image.h"

SimpleTexture TextureManager::missingTexture(-1);

TextureManager::TextureManager()
{
    const unsigned char missingTexData[] = {
        255, 0, 220, // purple, top left
        0,   0,   0, // black, top right
        0,   0,   0, // black, bottom left
        255, 0, 220  // purple, bottom right
    };

    GLuint missingTexId = -1;
    glGenTextures(1, &missingTexId);
    createTexture(missingTexId, missingTexData, GL_RGB, 2, 2);

    missingTexture.texId = missingTexId;
}

SimpleTexture TextureManager::loadTexture(Resource& r)
{
    const std::string path = r.getResourcePath() + ".png";

    const GLuint texId = textureFromFile(path);

    if (texId != INT_MAX)
    {
        SimpleTexture texObj(texId);

        textures.insert(std::pair<Resource, SimpleTexture>(r, texObj));

        return texObj;
    }
    else
    {
        LOG_ERROR("Failed to generate texture ID for %s", path);

        return missingTexture;
    }
}

TickableTexture TextureManager::loadAnimatedTexture(Resource& r, unsigned int textureTickLength,
                                                    const unsigned int textureFrameCount)
{
    std::string path = r.getResourcePath();

    std::vector<GLuint> textureIds;

    for (unsigned int i = 0; i < textureFrameCount; i++)
    {
        std::stringstream ss(path);
        ss << i << ".png";
        std::string currentPath = ss.str();

        GLuint currentTextureId = textureFromFile(currentPath);

        if (currentTextureId != INT_MAX)
        {
            textureIds.push_back(currentTextureId);
        }
        else
        {
            LOG_ERROR("Failed to generate texture ID for animated texture frame %i at %s", textureFrameCount,
                      currentPath);

            textureIds.push_back(missingTexture.texId);
        }
    }

    tickableTextures.emplace_back(textureIds, path, textureTickLength);
    textures.insert(std::pair<Resource, TickableTexture>(r, tickableTextures.back()));

    return tickableTextures.back();
}

void TextureManager::bindTexture(Resource& r)
{
    const SimpleTexture& tex = get(r);

    tex.bind();
}

SimpleTexture& TextureManager::get(Resource& r)
{
    const auto f = textures.find(r);

    SimpleTexture tex;

    if (f == textures.end())
    {
        // resource not found in already existing storage, needs to be loaded
        tex = loadTexture(r);
    }
    else
    {
        tex = (*f).second;
    }

    return tex;
}

// Called every tick, calls tick on every tickable texture.
void TextureManager::tickAllTextures()
{
    for (TickableTexture& tickableTexture : tickableTextures)
    {
        tickableTexture.tick();
    }
}

void TextureManager::createTexture(const GLuint& texId, const unsigned char* data, const GLenum& format,
                                   const unsigned int& width, const unsigned int& height)
{
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

GLuint TextureManager::textureFromFile(const std::string& filename)
{
    GLuint tex;
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

        createTexture(tex, data, format, width, height);

        stbi_image_free(data);

        return tex;
    }
    else
    {
        LOG_ERROR("Texture failed to load at path: %s", filename);
        stbi_image_free(data);

        return INT_MAX;
    }
}
