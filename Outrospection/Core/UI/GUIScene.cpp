#include "GUIScene.h"

#include "Outrospection.h"
#include "UIButton.h"
#include "Core/UI/GUIControlsOverlay.h"
#include <AllLevels.h>

// this is the constructor (ctor for short).
// it only takes care of copying the level data to store it here for now
GUIScene::GUIScene() : GUILayer("Scene", false),
	floor("platform", 0, 0, 0.1, 0.1),
	ink("hole", 0, 0, 0.1, 0.1),
	flag("flag", animatedTexture({"UI/flag/", "default"}, 10, 2), {0, 0}, {0, 0}),
	playerSprite("player/default", 0, 0, 0.1, 0.1)
	
{
	handleManually = true;

	playerSprite.addAnimation("die", simpleTexture({"UI/player/", "sad"}));
	playerSprite.addAnimation("win", simpleTexture({"UI/player/", "happy"}));

	setLevel(ALL_LEVELS[levelID]);
}

void GUIScene::setLevel(Level& lvl)
{
	level = std::move(lvl);
	playerPosInt = level.start;

	Util::doLater([this]{
		this->reset();
	}, 100);
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

	playerSprite.setScalePx(80, 65);
	floor.setScalePx(80, 65);
	ink.setScalePx(80, 65);
	flag.setScalePx(80, 65);
	
	for (int i = 0; i < level.data.length(); i++)
	{
		char tile = level.data[i];
		
		int xPos = i % level.rowLength;
		int yPos = i / level.rowLength;

		int xSpritePos = xPos * 16 * 4.6;
		int ySpritePos = yPos * 16 * 4.15;
		
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
	
	int xPlayerPos = playerPos.x * 16 * 4.6;
	int yPlayerPos = playerPos.y * 16 * 4.15;
	
	playerSprite.setPositionPx(xPlayerPos, yPlayerPos);
	playerSprite.draw(spriteShader, glyphShader);

	int xFlagPos = level.goal.x * 16 * 4.6;
	int yFlagPos = level.goal.y * 16 * 4.15;
	
	flag.setPositionPx(xFlagPos, yFlagPos);
	flag.draw(spriteShader, glyphShader);
}

void GUIScene::tryMovePlayer(Control input)
{
	if(!canMove)
	{
		LOG("Can't move your body.");
		return;
	}
	
	std::vector<glm::vec2> deltas;
	switch(input)
	{
	case Control::DASH_UP:
		deltas.emplace_back(0.0f, -1.0f);
	case Control::MOVE_UP:
		deltas.emplace_back(0.0f, -1.0f);
		break;
	case Control::DASH_DOWN:
		deltas.emplace_back(0.0f, 1.0f);
	case Control::MOVE_DOWN:
		deltas.emplace_back(0.0f, 1.0f);
		break;
	case Control::DASH_RIGHT:
		deltas.emplace_back(1.0f, 0.0f);
	case Control::MOVE_RIGHT:
		deltas.emplace_back(1.0f, 0.0f);
		break;
	case Control::DASH_LEFT:
		deltas.emplace_back(-1.0f, 0.0f);
	case Control::MOVE_LEFT:
		deltas.emplace_back(-1.0f, 0.0f);
		break;
	default:
		LOG_ERROR("Unknown control %i", int(input));
	}

	glm::vec2 totalDelta = glm::vec2();
	
	for (glm::vec2& delta : deltas) {
		glm::vec2 ghostPlayerPos = playerPosInt + delta;

		if(ghostPlayerPos == level.goal)
		{
			LOG_INFO("You win!! :D");
			flag.hidden = true;
			playerSprite.setAnimation("win");
			canMove = false;
			Util::doLater([this]{
				this->levelID++;
				setLevel(ALL_LEVELS[this->levelID]);
				
				//LOG_INFO("Advancing to level %i...", this->levelID);
			}, 1000);
		}

		int intGhostPosition = ghostPlayerPos.x + (ghostPlayerPos.y * level.rowLength);

		if (level.data[intGhostPosition] != ' ')
		{
			playerSprite.setAnimation("die");
			canMove = false;
			LOG_INFO("TODO show death screen"); // TODO show death screen
			return;
		}

		totalDelta += delta;
	}
	
	
	playerPosInt = playerPosInt + totalDelta;
	
}

void GUIScene::reset()
{
	playerPosInt = level.start;
	canMove = true;
	playerSprite.setAnimation("default");
	flag.hidden = false;
	
	Outrospection::get().keyBinds.clear();

	Outrospection::get().popOverlay(Outrospection::get().controlsOverlay);
	Outrospection::get().controlsOverlay = new GUIControlsOverlay();
	Outrospection::get().pushOverlay(Outrospection::get().controlsOverlay);
}

