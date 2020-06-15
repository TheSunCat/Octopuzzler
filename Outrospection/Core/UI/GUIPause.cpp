#include "GUIPause.h"

GUIPause::GUIPause() : GUIScreen("Pause Menu", false), buttonResume("button", .45f, .45f, .1f, .1f)
{
	
}

void GUIPause::tick()
{
	buttonResume.tick();
}

void GUIPause::draw(Shader& shader, Shader& glyphShader) const
{
	buttonResume.draw(shader, glyphShader);
}