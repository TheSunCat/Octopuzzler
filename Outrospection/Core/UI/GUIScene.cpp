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
	// test, fill up entire CRT
	wall.setPosition(0.17, 0.13);
	wall.setScale(0.56, 0.88);

	wall.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);

	return;
	
	for (auto& tile : level)
	{
		
		
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
