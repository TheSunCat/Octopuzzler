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
	if (Outrospection::get().won)
		return;
	
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
			
		case 'H': // ink
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
		LOG("Can't move your body."); // Undertale reference!!!!!!
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

	glm::vec2 ghostPlayerPos = playerPosInt;
	
	for (int i = 0; i < deltas.size(); i++) {
		glm::vec2& delta = deltas[i];
		
		ghostPlayerPos += delta;

		if(ghostPlayerPos == level.goal)
		{
			playerPosInt = ghostPlayerPos;
			LOG_INFO("You win!! :D");
			flag.hidden = true;
			playerSprite.setAnimation("win");
			Util::doLater([this] {this->canMove = false; }, 100);
			
			levelID++;
			
			if (levelID > sizeof(ALL_LEVELS) / sizeof(*ALL_LEVELS) - 1)
				Outrospection::get().won = true;
			
			Util::doLater([this]{
				
				if(this->levelID > (sizeof(ALL_LEVELS) / sizeof(*ALL_LEVELS)) - 1) // no more levels
				{
					auto& o = Outrospection::get();

					o.won = true;
					o.popLayer(o.scene);
					o.scene = nullptr;
					o.pushOverlay(o.winOverlay);
				} else
				{
					setLevel(ALL_LEVELS[this->levelID]);
					
					LOG_INFO("Advancing to level %i...", this->levelID);
					
				}
			}, 1000);
			return;
		}

		int intGhostPosition = ghostPlayerPos.x + (ghostPlayerPos.y * level.rowLength);
		char tile = level.data[intGhostPosition];
		
		if (tile != ' ' && !(tile == 'H' && i == 0 && deltas.size() == 2))
		{
			LOG_INFO("Player died!");
			
			playerPosInt = ghostPlayerPos;
			playerSprite.setAnimation("die");

			Util::doLater([this] {this->canMove = false; }, 100);
			Util::doLater([this] {this->reset(); }, 1500);
			
			//LOG_INFO("TODO show death screen"); // TODO show death screen
			return;
		}

		totalDelta += delta;
	}
	
	
	playerPosInt += totalDelta;
}

void GUIScene::reset()
{
	playerPosInt = level.start;
	canMove = true;
	playerSprite.setAnimation("default");
	flag.hidden = false;
	
	Outrospection::get().inputQueue.clear();
	
	((GUIControlsOverlay*)Outrospection::get().controlsOverlay)->setControls(level.controls);
}

