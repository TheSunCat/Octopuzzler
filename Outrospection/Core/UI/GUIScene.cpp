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
	//wall.setPosition(0, 0);
	//wall.setScale(1, 0.5);

	//wall.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);

	//return;

	float tileSize = 0.9f / rowLength;
	player.setScale(tileSize, tileSize);
	wall.setScale(tileSize, tileSize);
	hole.setScale(tileSize, tileSize);
	
	for (int i = 0; i < level.length(); i++)
	{
		char tile = level[i];
		
		int xPos = i % rowLength;
		int yPos = i / rowLength;

		float xSpritePos = xPos * tileSize;
		float ySpritePos = yPos * tileSize;
		
		switch(tile)
		{
		case ' ': // empty space. skip
			break;
		case 'w': // wall
			// TODO actually draw a wall
			
			break;
		case 'o': // hole
			// TODO actually draw a hole
			break;
		}
		
		//button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
	}

	
}
