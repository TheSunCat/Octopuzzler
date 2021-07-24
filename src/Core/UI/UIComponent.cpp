#include "UIComponent.h"

#include <utility>

#include "Outrospection.h"
#include "Util.h"

UITransform::UITransform(int posX, int posY, int sizeX, int sizeY,
                         const glm::vec2& fbRes, UIAlign _alignment)
    : pos(posX, posY), size(sizeX, sizeY), defaultRes(fbRes), alignment(_alignment)
{
    switch(alignment)
    {
    case UIAlign::CENTER:
        pos -= size / 2.f;
        break;

    case UIAlign::TOP_LEFT:

        break;

    case UIAlign::TOP_RIGHT:
        pos.x -= size.x;
        break;

    case UIAlign::BOT_LEFT:
        pos.y -= size.y;
        break;

    case UIAlign::BOT_RIGHT:
        pos -= size;
        break;
    }
}

glm::vec2 UITransform::getPos() const
{
    return pos * glm::vec2(*Outrospection::get().curFbResolution / defaultRes);
}

glm::vec2 UITransform::getSize() const
{
    return size * (*Outrospection::get().curFbResolution / defaultRes);
}

void UITransform::setPos(int x, int y)
{
    pos = glm::vec2(x, y);
}

void UITransform::setSize(int x, int y)
{
    size = glm::vec2(x, y);
}

GLuint UIComponent::quadVAO = 0;

UIComponent::UIComponent(const std::string& _texName, const GLint& texFilter, const UITransform& _transform)
    : UIComponent(_texName, simpleTexture({"UI/", _texName}, texFilter), _transform)
{
}

UIComponent::UIComponent(std::string _name, SimpleTexture& _tex, const UITransform& _transform)
    : name(std::move(_name)), textOffset(0.0f, _transform.getSize().y / 2), textColor(0.0f), transform(_transform)
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

    if(animations.find(anim) == animations.end())
    {
        LOG_ERROR("Animation %s has not been loaded!", anim);
        curAnimation = "default";
    } else {
        curAnimation = anim;
    }

    animations.at(curAnimation)->reset();
    animations.at(curAnimation)->shouldTick = true;
}

void UIComponent::setPosition(int x, int y)
{
    transform.setPos(x, y);
}

void UIComponent::setScale(int px)
{
    transform.setSize(px, px);
}

void UIComponent::setScale(int x, int y)
{
    transform.setSize(x, y);
}

void UIComponent::draw(Shader& shader, const Shader& glyphShader) const
{
    if (hidden)
        return;

    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(transform.getPos(), 0));
    model = glm::scale(model, glm::vec3(transform.getSize(), 0));

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

    float textX = transform.getPos().x + textOffset.x;
    float textY = transform.getPos().y + textOffset.y;

    for (char c : text)
    {
        if (c <= '\0' || c == ' ')
        {
            textX += 10;
            continue;
        }

        if (!Outrospection::get().fontCharacters.contains(c)) {
            LOG_ERROR("Character %c not found!", c);
            continue;
        }

        FontCharacter fontCharacter = Outrospection::get().fontCharacters[c];

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
