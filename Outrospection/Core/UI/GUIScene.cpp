#include "GUIScene.h"

#include "Outrospection.h"
#include "UIButton.h"

// this is the constructor (ctor for short).
// it only takes care of copying the level data to store it here for now
GUIScene::GUIScene(Level& _level) : GUILayer("Scene", false),
	level(std::move(_level)), playerPosInt(level.start),
	floor("floor/single", 0, 0, 0.1, 0.1),
	ink("ink/single", 0, 0, 0.1, 0.1),
	playerSprite("player/default", 0, 0, 0.1, 0.1)
	
{
	handleManually = true;


	playerSprite.addAnimation("die", simpleTexture({"UI/player/", "sad"}));
}

void GUIScene::tick()
{
	playerPos = Util::lerp(playerPos, playerPosInt, 0.2);
	
	floor.tick();
	ink.tick();
	
	playerSprite.tick();
}

void GUIScene::draw() const
{
	Shader& spriteShader = Outrospection::get().spriteShader;
	Shader& glyphShader = Outrospection::get().glyphShader;

	playerSprite.setScalePx(160, 130);
	floor.setScalePx(160, 130);
	ink.setScalePx(160, 130);
	
	for (int i = 0; i < level.data.length(); i++)
	{
		char tile = level.data[i];
		
		int xPos = i % level.rowLength;
		int yPos = i / level.rowLength;

		int xSpritePos = xPos * 16 * 9.2;
		int ySpritePos = yPos * 16 * 8.3;
		
		switch(tile)
		{
		case ' ': // floor
			floor.setPositionPx(xSpritePos, ySpritePos);
			floor.draw(spriteShader, glyphShader);

			break;
			
		case 'w': // wall
			break;
			
		case 'i': // ink
			ink.setPositionPx(xSpritePos, ySpritePos);
			ink.draw(spriteShader, glyphShader);
			break;
		}
	}
	
	int xPlayerPos = playerPos.x * 16 * 9.2;
	int yPlayerPos = playerPos.y * 16 * 8.3;
	
	playerSprite.setPositionPx(xPlayerPos, yPlayerPos);
	playerSprite.draw(spriteShader, glyphShader);
}

void GUIScene::tryMovePlayer(Control input)
{
	std::vector<glm::vec2> deltas;
	switch(input)
	{
	case Control::DASH_UP:
		deltas.push_back(glm::vec2(0.0f, -1.0f));
	case Control::MOVE_UP:
		deltas.push_back(glm::vec2(0.0f, -1.0f));
		break;
	case Control::DASH_DOWN:
		deltas.push_back(glm::vec2(0.0f, 1.0f));
	case Control::MOVE_DOWN:
		deltas.push_back(glm::vec2(0.0f, 1.0f));
		break;
	case Control::DASH_RIGHT:
		deltas.push_back(glm::vec2(1.0f, 0.0f));
	case Control::MOVE_RIGHT:
		deltas.push_back(glm::vec2(1.0f, 0.0f));
		break;
	case Control::DASH_LEFT:
		deltas.push_back(glm::vec2(-1.0f, 0.0f));
	case Control::MOVE_LEFT:
		deltas.push_back(glm::vec2(-1.0f, 0.0f));
		break;
	default:
		LOG_ERROR("Unknown control %i", int(input));
	}

	glm::vec2 totalDelta = glm::vec2();
	
	for (glm::vec2& delta : deltas) {
		glm::vec2 ghostPlayerPos = playerPosInt + delta;

		int intGhostPosition = ghostPlayerPos.x + (ghostPlayerPos.y * level.rowLength);

		if (level.data[intGhostPosition] != ' ')
		{
			playerSprite.setAnimation("die");
			return;
		}

		totalDelta += delta;
	}
	
	
	playerPosInt = playerPosInt + totalDelta;
	
}
