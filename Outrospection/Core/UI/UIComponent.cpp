#include "UIComponent.h"

#include <utility>
#include <glm/ext/matrix_transform.inl>

#include "Source.h"

GLuint UIComponent::quadVAO = 0;

UIComponent::UIComponent(const std::string& _texName, const float posXPercent, const float posYPercent,
                         const glm::vec2 dimensions)
	: UIComponent(_texName, glm::vec2(SCR_WIDTH* posXPercent, SCR_HEIGHT* posYPercent), dimensions * glm::vec2(SCR_WIDTH, SCR_HEIGHT))
{ }

UIComponent::UIComponent(std::string _texName, const glm::vec2& _position, const glm::vec2& dimensions)
	: name(std::move(_texName)), position(_position), width(dimensions.x), height(dimensions.y)
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

void UIComponent::draw(Shader& shader) const
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
}
