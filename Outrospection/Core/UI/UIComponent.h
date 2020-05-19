#pragma once
#include <string>

#include <glm/vec2.hpp>

#include "Core/Rendering/SimpleTexture.h"
#include "Core/Rendering/TextureManager.h"

class Shader;

class UIComponent
{
public:
	UIComponent(const std::string& _texName, const float posXPercent, const float posYPercent, const glm::vec2 dimensions);
	
	UIComponent(std::string _texName, const glm::vec2& _position, const glm::vec2& dimensions);

	void draw(Shader& shader) const;
	
	std::string name;
	glm::vec2 position;
	float width = 0.0f, height = 0.0f;
private:
	SimpleTexture texture = TextureManager::missingTexture;

	static GLuint quadVAO;
};
