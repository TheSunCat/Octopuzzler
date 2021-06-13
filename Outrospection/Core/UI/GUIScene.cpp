#include "GUIScene.h"

#include "Outrospection.h"
#include "UIButton.h"

// this is the constructor (ctor for short).
// it only takes care of copying the level data to store it here for now
GUIScene::GUIScene(std::string& _level, int _rowLength) : GUILayer("Scene", false),
	level(std::move(_level)), rowLength(_rowLength),
	wall("wall/single", 0, 0, 0.1, 0.1),
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


	Shader& spriteShader = Outrospection::get().spriteShader;
	Shader& glyphShader = Outrospection::get().glyphShader;

	float xTileSize = 1.8f / float(rowLength);
	float yTileSize = 2.0f / int(level.length() / rowLength);

	player.setScale(xTileSize, yTileSize);
	wall.setScale(xTileSize, yTileSize);
	hole.setScale(xTileSize, yTileSize);
	
	for (int i = 0; i < level.length(); i++)
	{
		char tile = level[i];
		
		int xPos = i % rowLength;
		int yPos = i / rowLength;

		float xSpritePos = xPos * xTileSize;
		float ySpritePos = 1 - yPos * yTileSize;
		
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

		printf("%i, %i\n", xPos, yPos);
		
		//button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
	}

	//__debugbreak();

	
}
