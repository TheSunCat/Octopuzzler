#pragma once
#include <string>

#include <glm/vec2.hpp>

#include "Core/Rendering/SimpleTexture.h"
#include "Core/Rendering/TextureManager.h"

class Shader;

class UIComponent
{
public:
	UIComponent(const std::string& _texName, const float posXPercent, const float posYPercent, const float widthPercent, const float heightPercent);
	
	UIComponent(std::string _texName, const glm::vec2& _position, const glm::vec2& dimensions);

	virtual void draw(Shader& shader, const Shader& glyphShader) const;

	virtual void drawText(const std::string& text, const Shader& glyphShader) const;
	
	virtual void tick();
	
	std::string name;
	glm::vec2 position;
	float width = 10.0f, height = 10.0f;
	glm::vec2 textOffset; // vector to offset text by

	Color textColor;

	virtual ~UIComponent() = default;
private:
	SimpleTexture texture = TextureManager::missingTexture;

	static GLuint quadVAO;
};
