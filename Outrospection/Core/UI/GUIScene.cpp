#include "GUIScene.h"

#include "Outrospection.h"
#include "UIButton.h"

// this is the constructor (ctor for short).
// it only takes care of copying the level data to store it here for now
GUIScene::GUIScene(std::string& _level, int _rowLength) : GUILayer("Scene", false),
	level(std::move(_level)), rowLength(_rowLength),
	wall("wallTile", 0, 0, 0.1, 0.1),
	hole("holeTile", 0, 0, 0.1, 0.1),
	player("player", 0, 0, 0.1, 0.1)
	
{
	handleManually = true;
}

void GUIScene::tick()
{
	wall.tick();
	hole.tick();
	
	player.tick();
}

void GUIScene::draw() const
{
	// fill entire framebuffer with missing texture test
	wall.setPosition(0, 0);
	wall.setScale(1, 1);

	wall.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);

	return;

	Shader& spriteShader = Outrospection::get().spriteShader;
	Shader& glyphShader = Outrospection::get().glyphShader;

	float tileSize = 0.45f / rowLength;
	player.setScale(tileSize, tileSize * 1.5);
	wall.setScale(tileSize, tileSize * 1.5);
	hole.setScale(tileSize, tileSize * 1.5);
	
	for (int i = 0; i < level.length(); i++)
	{
		char tile = level[i];
		
		int xPos = i % rowLength;
		int yPos = i / rowLength;

		float xSpritePos = 0.275 + xPos * tileSize;
		float ySpritePos = 0.25 + yPos * (tileSize * 1.5);
		
		switch(tile)
		{
		case ' ': // empty space. skip
			break;
		case 'w': // wall
			// TODO actually draw a wall
			wall.setPosition(xSpritePos, ySpritePos);
			wall.draw(spriteShader, glyphShader);
			break;
		case 'o': // hole
			// TODO actually draw a hole
			hole.setPosition(xSpritePos, ySpritePos);
			hole.draw(spriteShader, glyphShader);
			break;
		}
		
		//button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
	}

	
}
