#include "GUIScene.h"

#include "Outrospection.h"
#include "UIButton.h"

// this is the constructor (ctor for short).
// it only takes care of copying the level data to store it here for now
GUIScene::GUIScene(Level& _level) : GUILayer("Scene", false),
	level(std::move(_level)), playerPosInt(level.start),
	wall("wall/single", 0, 0, 0.1, 0.1),
	hole("holeTile", 0, 0, 0.1, 0.1),
	playerSprite("player/sad0", 0, 0, 0.1, 0.1)
	
{
	handleManually = true;
}

void GUIScene::tick()
{
	playerPos = Util::lerp(playerPos, playerPosInt, 0.2);
	
	wall.tick();
	hole.tick();
	
	playerSprite.tick();
}

void GUIScene::draw() const
{
	Shader& spriteShader = Outrospection::get().spriteShader;
	Shader& glyphShader = Outrospection::get().glyphShader;

	playerSprite.setScalePx(160, 130);
	wall.setScalePx(160, 130);
	hole.setScalePx(160, 130);

	int xPlayerPos = playerPos.x * 16 * 9.2;
	int yPlayerPos = playerPos.y * 16 * 8.3;
	
	playerSprite.setPositionPx(xPlayerPos, yPlayerPos);
	playerSprite.draw(spriteShader, glyphShader);
	
	for (int i = 0; i < level.data.length(); i++)
	{
		char tile = level.data[i];
		
		int xPos = i % level.rowLength;
		int yPos = i / level.rowLength;

		int xSpritePos = xPos * 16 * 9.2;
		int ySpritePos = yPos * 16 * 8.3;
		
		switch(tile)
		{
		case ' ': // empty space. skip
			break;
			
		case 'w': // wall
			wall.setPositionPx(xSpritePos, ySpritePos);
			wall.draw(spriteShader, glyphShader);
			break;
			
		case 'o': // hole
			hole.setPositionPx(xSpritePos, ySpritePos);
			hole.draw(spriteShader, glyphShader);
			break;
		}
	}
}

void GUIScene::tryMovePlayer(Control input)
{
	glm::vec2 delta = glm::vec2();
	switch(input)
	{
	case Control::MOVE_UP:
		delta.y--;
		break;
	case Control::MOVE_DOWN:
		delta.y++;
		break;
	case Control::MOVE_RIGHT:
		delta.x++;
		break;
	case Control::MOVE_LEFT:
		delta.x--;
		break;
	default:
		LOG_ERROR("Unknown control %i", int(input));
	}

	glm::vec2 ghostPlayerPos = playerPosInt + delta;

	int intGhostPosition = ghostPlayerPos.x + (ghostPlayerPos.y * level.rowLength);
	
	if(level.data[intGhostPosition] != ' ')
	{
		LOG("TODO: Kill player");
	} else
	{
		playerPosInt = playerPosInt + delta;
	}
	
}
