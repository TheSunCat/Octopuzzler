#include "GUIScene.h"

#include "Outrospection.h"
#include "UIButton.h"

// this is the constructor (ctor for short).
// it only takes care of copying the level data to store it here for now
GUIScene::GUIScene(std::string& _level, int _rowLength) : GUILayer("Scene", false),
	level(std::move(_level)), rowLength(_rowLength),
	wall("wall/single", 0, 0, 0.1, 0.1),
	hole("holeTile", 0, 0, 0.1, 0.1),
	player("player/sad0", 0, 0, 0.1, 0.1)
	
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

	player.setScalePx(160, 130);
	wall.setScalePx(160, 130);
	hole.setScalePx(160, 130);

	int xPlayerPos = playerPos.x * 16 * 9.2;
	int yPlayerPos = playerPos.y * 16 * 8.3;
	
	player.setPositionPx(xPlayerPos, yPlayerPos);
	player.draw(spriteShader, glyphShader);
	
	for (int i = 0; i < level.length(); i++)
	{
		char tile = level[i];
		
		int xPos = i % rowLength;
		int yPos = i / rowLength;

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

		//printf("%i, %i\n", xPos, yPos);
		
		//button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
	}

	//__debugbreak();

	
}
