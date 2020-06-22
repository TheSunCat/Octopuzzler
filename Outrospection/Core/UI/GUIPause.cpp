#include "GUIPause.h"
#include <Outrospection.h>

GUIPause::GUIPause() : GUIScreen("Pause Menu", false), buttonResume("button", .45f, .45f, .1f, .1f, Outrospection::unpauseGame)
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