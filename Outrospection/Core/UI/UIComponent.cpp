#include "UIComponent.h"

#include <utility>
#include <glm/ext/matrix_transform.inl>

#include "Source.h"

GLuint UIComponent::quadVAO = 0;

UIComponent::UIComponent(const std::string& _texName, const float posXPercent, const float posYPercent,
	const float widthPercent, const float heightPercent)
	: UIComponent(_texName, glm::vec2(SCR_WIDTH* posXPercent, SCR_HEIGHT* posYPercent),
		glm::vec2(widthPercent * SCR_WIDTH, heightPercent * SCR_HEIGHT))
{ }

UIComponent::UIComponent(std::string _texName, const glm::vec2& _position, const glm::vec2& dimensions)
	: name(std::move(_texName)), position(_position), width(dimensions.x), height(dimensions.y), textOffset(0.0f, height / 2), textColor(0.0f)
{
	TextureManager& _textureManager = getOutrospection()->textureManager;
	Resource r("UI/", name + ".png");
	texture = _textureManager.get(r);

	// we need to create our quad the first time!
	if(quadVAO == 0)
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

void UIComponent::draw(Shader& shader, const Shader& glyphShader) const
{
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));

	// rotation code, no need to worry about this for now
	//model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	//model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
	
	model = glm::scale(model, glm::vec3(width, height, 0));

	shader.setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	if(!name.empty()) // drawing item name for now, need a text class from other branch
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

	for (char c : name)
	{
		if (c == '\0') // replace null char with space
			c = ' ';

		FontCharacter fontCharacter = getOutrospection()->fontCharacters.at(c);

		// calculate model matrix
		glm::mat4 charModel = glm::mat4(1.0f);

		float xPos = textX + fontCharacter.bearing.x * textScale;
		float yPos = textY - fontCharacter.bearing.y * textScale;
		charModel = glm::translate(charModel, glm::vec3(xPos, yPos, 0.0f));

		float width = (fontCharacter.size.x) * textScale;
		float height = (fontCharacter.size.y) * textScale;
		charModel = glm::scale(charModel, glm::vec3(width, height, 1.0f));

		glyphShader.setMat4("model", charModel);
		glBindTexture(GL_TEXTURE_2D, fontCharacter.textureId);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		textX += (fontCharacter.advance >> 6) * textScale;
	}

	glBindVertexArray(0);
}

void UIComponent::tick() {}