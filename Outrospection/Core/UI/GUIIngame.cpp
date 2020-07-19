#include "GUIIngame.h"

#include "Outrospection.h"

GUIIngame::GUIIngame() : GUILayer("Ingame GUI", true), healthDisplay("health", 0, 0, .10f, .10f),
                         posX("button", 0, .10f, .10f, .10f),
                         posY("button", 0, .20f, .10f, .10f),
                         posZ("button", 0, .30f, .10f, .10f)
{
}

void GUIIngame::tick()
{
    Outrospection& instance = Outrospection::get();
    posX.name = std::to_string(instance.player.position.x);
    posY.name = std::to_string(instance.player.position.y);
    posZ.name = std::to_string(instance.player.position.z);
    
    posX.tick();
    posY.tick();
    posZ.tick();
}

void GUIIngame::draw() const
{
    healthDisplay.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    
    posX.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    posY.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    posZ.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
}