#include "UIComponent.h"

#include <utility>

#include "Outrospection.h"
#include "Util.h"

GLuint UIComponent::quadVAO = 0;

UIComponent::UIComponent(const std::string& _texName, const GLint& texFilter, const float posXPercent,
                         const float posYPercent,
                         const float widthPercent, const float heightPercent)
    : UIComponent(_texName, texFilter, glm::vec2(SCR_WIDTH * posXPercent, SCR_HEIGHT * posYPercent),
                  glm::vec2(widthPercent * SCR_WIDTH, heightPercent * SCR_HEIGHT))
{
}

UIComponent::UIComponent(std::string _texName, const GLint& texFilter, const glm::vec2& _position,
                         const glm::vec2& dimensions)
    : UIComponent(std::move(_texName), simpleTexture({"UI/", _texName}, texFilter), _position, dimensions)
{
}

UIComponent::UIComponent(std::string _name, SimpleTexture& _tex, const glm::vec2& _position,
                         const glm::vec2& dimensions)
    : name(std::move(_name)), textOffset(0.0f, dimensions.y / 2), textColor(0.0f), position(_position),
      width(dimensions.x),
      height(dimensions.y)
{
    animations.insert(std::make_pair("default", &_tex));

    // we need to create our quad the first time!
    if (quadVAO == 0)
    {
        // configure VAO/VBO
        GLuint VBO;
        float vertices[] = {
            0.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,

            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(quadVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void UIComponent::tick()
{
}

void UIComponent::addAnimation(const std::string& anim, SimpleTexture& _tex)
{
    animations.insert(std::make_pair(anim, &_tex));
}

void UIComponent::setAnimation(const std::string& anim)
{
    animations.at(curAnimation)->shouldTick = false;
    animations.at(curAnimation)->reset();
    curAnimation = anim;
    animations.at(curAnimation)->shouldTick = true;
}

void UIComponent::setPosition(float xPercent, float yPercent)
{
    setPositionPx(xPercent * SCR_WIDTH, yPercent * SCR_HEIGHT);
}

void UIComponent::setScale(float xPercent, float yPercent)
{
    setScalePx(xPercent * SCR_WIDTH, yPercent * SCR_HEIGHT);
}

void UIComponent::setPositionPx(int x, int y)
{
    position = glm::vec2(x, y);
}

void UIComponent::setScalePx(int scale)
{
    setScalePx(scale, scale);
}

void UIComponent::setScalePx(int _width, int _height)
{
    width = _width, height = _height;
}

void UIComponent::draw(Shader& shader, const Shader& glyphShader) const
{
    if (hidden)
        return;

    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));

    // rotation, no need to worry about this for now
    //model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    //model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    //model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(width, height, 0));

    shader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    animations.at(curAnimation)->bind();

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    if (showText && !name.empty()) // TODO text class
    {
        drawText(name, glyphShader);
    }
}

void UIComponent::drawText(const std::string& text, const Shader& glyphShader) const
{
    glyphShader.use();
    glyphShader.setVec3("textColor", textColor); // temp set color to black
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(quadVAO);

    float textScale = 1.0f;

    float textX = position.x + textOffset.x;
    float textY = position.y + textOffset.y;

    for (char c : text)
    {
        if (c <= '\0' || c == ' ')
        {
            textX += 10;
            continue;
        }


        FontCharacter fontCharacter = Outrospection::get().fontCharacters.at(c);

        // calculate model matrix
        glm::mat4 charModel = glm::mat4(1.0f);

        float xPos = textX + fontCharacter.bearing.x * textScale + 10;
        float yPos = textY - fontCharacter.bearing.y * textScale + 10; // TODO these +s at the end shouldn't be needed
        charModel = glm::translate(charModel, glm::vec3(xPos, yPos, 0.0f));

        float width = (fontCharacter.size.x) * textScale;
        float height = (fontCharacter.size.y) * textScale;
        charModel = glm::scale(charModel, glm::vec3(width, height, 1.0f));

        // TODO not hardcode this lol
        if (c == 'C' || c == 'S' || c == 'T')
            glyphShader.setVec3("textColor", glm::vec3(0.0549f, 0.0902f, 0.1725f)); //0x0E172C
        else
            glyphShader.setVec3("textColor", glm::vec3(0.8941f, 0.2039f, 0.4314f)); //0xE4346E

        glyphShader.setMat4("model", charModel);
        glBindTexture(GL_TEXTURE_2D, fontCharacter.textureId);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        textX += (fontCharacter.advance >> 6) * textScale;
    }

    glBindVertexArray(0);
}
