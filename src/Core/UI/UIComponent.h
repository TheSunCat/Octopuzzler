#pragma once
#include <string>

#include <glm/vec2.hpp>

#include "Core/Rendering/SimpleTexture.h"
#include "Core/Rendering/TextureManager.h"

class Shader;

class UIComponent
{
public:
    UIComponent(const std::string& _texName, const GLint& texFilter,
                float posXPercent, float posYPercent,
                float widthPercent, float heightPercent);


    UIComponent(const std::string& _name, SimpleTexture& _tex,
                const float posXPercent, const float posYPercent,
                const float widthPercent, const float heightPercent);

    UIComponent(std::string _texName, const GLint& texFilter,
                const glm::vec2& _position, const glm::vec2& dimensions);

    UIComponent(std::string _name, SimpleTexture& _tex,
                const glm::vec2& _position, const glm::vec2& dimensions);

    virtual void draw(Shader& shader, const Shader& glyphShader) const;

    virtual void tick();

    void addAnimation(const std::string& anim, SimpleTexture& _tex);
    void setAnimation(const std::string& anim);

    void setPosition(float xPercent, float yPercent);
    void setScale(float xPercent, float yPercent);

    void setPositionPx(int x, int y);
    void setScalePx(int scale);
    void setScalePx(int _width, int _height);
    std::string name;

    glm::vec2 textOffset; // vector to offset text by
    bool showText = false;
    Color textColor;

    bool hidden = false;

    virtual ~UIComponent() = default;
private:
    glm::vec2 position;
    float width = 10.0f, height = 10.0f;

    virtual void drawText(const std::string& text, const Shader& glyphShader) const;

    std::string curAnimation = "default";
    std::unordered_map<std::string, SimpleTexture*> animations;

    static GLuint quadVAO;
};
