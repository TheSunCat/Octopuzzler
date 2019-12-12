#include "Billboard.h"

Billboard::Billboard()
{
	float quadVertices[] = {  // tex coords = (x + 0.5, y)
		-0.5f,  0.0f,
		 0.5f,  0.0f,
		 0.5f,  1.0f,

		-0.5f,  0.0f,
		 0.5f,  1.0f,
		-0.5f,  1.0f
	};

	unsigned int quadVBO = 0;

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	// vertex coordinates
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
}

void Billboard::draw(Shader& _shader, const glm::vec3& _pos, unsigned int _texture) {
	_shader.use();

	_shader.setInt("tex", 0);

	// texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	// Billboard rotation
	_shader.setVec3("billboardCenter", _pos);
	
	glBindVertexArray(quadVAO);

	glm::mat4 modelMat = glm::translate(glm::mat4(1.0), _pos);
	_shader.setMat4("model", modelMat);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Billboard::draw(Shader& _shader, const glm::vec3& _pos, const Animation& a) {
	_shader.use();

	unsigned int texId = a.frames[a.curFrame];

	draw(_shader, _pos, texId);
}