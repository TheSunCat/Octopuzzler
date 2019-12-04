#include "Character.h"
#include "Model.h"

Character::Character(const std::string& _charId)
{
	charId = _charId;

	float quadVertices[] = {  // tex coords = (x + 0.5, y)
		-0.5f,  0.0f,  0.0f,
		 0.5f,  0.0f,  0.0f,
		 0.5f,  1.0f,  0.0f,
		-0.5f,  1.0f,  0.0f
	};

	// screen quad VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	// vertex coordinates
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	charTexture = TextureFromFile((charId + ".png").c_str(), "./res/ObjectData/Characters", false);
}

void Character::draw(Shader& _shader)
{
	_shader.use();
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, charTexture);
	glDrawArrays(GL_TRIANGLES, 6, 4);
}