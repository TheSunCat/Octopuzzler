#include "GUIScene.h"

#include "Outrospection.h"
#include "UIButton.h"
#include "Core/UI/GUIControlsOverlay.h"
#include <json.hpp>

// this is the constructor (ctor for short).
// it only takes care of copying the level data to store it here for now
GUIScene::GUIScene() : GUILayer("Scene", false),
                    floor("floor", animatedTexture({ "UI/floor/", "empty" }, 8, 17, GL_NEAREST), { 0, 0 }, { 0.1, 0.1 }),
                    ink("hole", GL_NEAREST, 0, 0, 0.1, 0.1),
                    flag("flag", animatedTexture({"UI/flag/", "default"}, 16, 2, GL_NEAREST), {0, 0}, {0, 0}),
                    background("background", animatedTexture({"UI/background/", "default"}, 8, 17, GL_NEAREST), {0, 0}, {0, 0}),
                    playerSprite("player", animatedTexture({ "UI/player/", "default" }, 16, 2, GL_NEAREST), { 0, 0 }, { 0.1, 0.1 }),
                    ghostSprite("ghost", animatedTexture({ "UI/ghost/", "default" }, 16, 2, GL_NEAREST), { 0, 0 }, { 0.1, 0.1 })

{
    handleManually = true;

    playerSprite.addAnimation("fail", simpleTexture({"UI/player/", "fail"}, GL_NEAREST));
    playerSprite.addAnimation("failInk", simpleTexture({"UI/player/", "failInk"}, GL_NEAREST));
    playerSprite.addAnimation("win", simpleTexture({"UI/player/", "win"}, GL_NEAREST));

    setLevel("", 3);
}

void GUIScene::setLevel(const std::string& lvlName, int lvlID)
{
    // parse level
    std::string levelData = Util::readAllBytes("StageData/level" + lvlName + lvlID);
    
    nlohmann::json jason = nlohmann::json::parse(levelData);
    level = jason.get<Level>();
    
    playerPosInt = level.start;

    Util::doLater([this]
    {
        this->reset();
    }, 100);
}

void GUIScene::tick()
{
    playerPos = Util::lerp(playerPos, playerPosInt, 0.2);

    if(!showGhost)
    {
        ghostPosInt = playerPosInt; ghostPos = ghostPosInt;
        curGhostMove = 0;
    } else
    {
        ghostPos = Util::lerp(ghostPos, ghostPosInt, 0.2);
    }

    if(playerPos == playerPosInt) // finished moving
    {
        showGhost = true;
    }

    floor.tick();
    ink.tick();

    playerSprite.tick();
}

void GUIScene::draw() const
{
    if (Outrospection::get().won) // don't draw the level if we won
        return;

    Shader& spriteShader = Outrospection::get().spriteShader;
    Shader& glyphShader = Outrospection::get().glyphShader;

    int rowLength = level.rowLength;
    int colLength = int(level.data.size() / level.rowLength);

    int largestLength = std::max(rowLength, colLength);

    float spriteScale = 4.f / largestLength;

    playerSprite.setScale(spriteScale, spriteScale);
    ghostSprite.setScale(spriteScale, spriteScale);
    floor.setScale(spriteScale + 0.01, spriteScale + 0.01); // adjust for floating point imprecision (I think?)
    ink.setScale(spriteScale, spriteScale + 0.01);   // else we get weird horizontal lines between some tiles
    flag.setScale(spriteScale, spriteScale);         // TODO looks like this isn't working. too bad.
    background.setScale(spriteScale + 0.01, spriteScale + 0.01);         

    for(int x = 0; x < largestLength + 5; x++) // TODO dirty hack
    {
        for(int y = 0; y < largestLength; y++)
        {
            float xSpritePos = x * spriteScale;
            float ySpritePos = y * spriteScale;

            background.setPosition(xSpritePos, ySpritePos);
            background.draw(spriteShader, glyphShader);

            if (x < (largestLength - rowLength) / 2 || x >= rowLength + (largestLength - rowLength) / 2 ||
                y < (largestLength - colLength) / 2 || y >= colLength + (largestLength - colLength) / 2) // tile not in level
                continue;

            char tile = level.data[(x - (largestLength - rowLength) / 2) + (y - (largestLength - colLength) / 2) * rowLength];

            switch (tile)
            {
            case ' ': // floor
                floor.setPosition(xSpritePos, ySpritePos);
                floor.draw(spriteShader, glyphShader);
                break;

            case 'W': // wall
                break;

            case 'H': // ink
                floor.setPosition(xSpritePos, ySpritePos);
                floor.draw(spriteShader, glyphShader);

                ink.setPosition(xSpritePos, ySpritePos);
                ink.draw(spriteShader, glyphShader);
                break;
            }
        }
    }

    float xFlagPos = (level.goal.x + (largestLength - rowLength) / 2) * spriteScale;
    float yFlagPos = (level.goal.y + (largestLength - colLength) / 2) * spriteScale;

    flag.setPosition(xFlagPos, yFlagPos);
    flag.draw(spriteShader, glyphShader);

    if (showGhost && canMove) {
        float xGhostPos = (ghostPos.x + (largestLength - rowLength) / 2) * spriteScale;
        float yGhostPos = (ghostPos.y + (largestLength - colLength) / 2) * spriteScale;

        ghostSprite.setPosition(xGhostPos, yGhostPos);
        ghostSprite.draw(spriteShader, glyphShader);
    }

    float xPlayerPos = (playerPos.x + (largestLength - rowLength) / 2) * spriteScale;
    float yPlayerPos = (playerPos.y + (largestLength - colLength) / 2) * spriteScale;

    playerSprite.setPosition(xPlayerPos, yPlayerPos);
    playerSprite.draw(spriteShader, glyphShader);
}

void GUIScene::worldTick()
{
    if (!inputQueue.empty())
    {
        // read input
        Control curInput = inputQueue[0];
        LOG_INFO("Player is playing %c", char(curInput));

        tryMovePlayer(curInput);

        // erase it so we move to the next input
        inputQueue.erase(inputQueue.begin());
    }

    if(curGhostMove >= 0 && !ghostInputQueue.empty())
    {
        // read input
        Control curInput = ghostInputQueue[curGhostMove];
        LOG_INFO("Ghost is playing %c", char(curInput));

        moveGhost(curInput);
    }

    // increment so we move to the next input
    curGhostMove++;

    if (curGhostMove == ghostInputQueue.size()) {
        curGhostMove = -10; // pause for tick
    }

    if(curGhostMove == 0) // about to start anim, reset ghost
    {
        ghostPosInt = playerPosInt;
        ghostPos = playerPosInt;
    }
}


void GUIScene::tryMovePlayer(Control input)
{
    if (!canMove)
    {
        LOG("Can't move your body."); // Undertale reference!!!!!!
        return;
    }

    pastPositions.emplace_back(playerPosInt);

    auto deltas = controlToDeltas(input);

    glm::vec2 totalDelta = glm::vec2();

    glm::vec2 ghostPlayerPos = playerPosInt;

    for (int i = 0; i < deltas.size(); i++)
    {
        glm::vec2& delta = deltas[i];

        ghostPlayerPos += delta;

        if (ghostPlayerPos == level.goal)
        {
            playerPosInt = ghostPlayerPos;
            LOG_INFO("You win!! :D");
            flag.hidden = true;
            playerSprite.setAnimation("win");
            Util::doLater([this] { this->canMove = false; }, 100);

            levelID++;

            Util::doLater([this]
            {
                if (!Util::fileExists("StageData/level" + std::to_string(this->levelID))) // no more levels
                {
                    auto& o = Outrospection::get();

                    o.won = true;
                    o.pushOverlay(o.winOverlay);
                }
                else
                {
                    setLevel("", this->levelID);

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

            if(tile == 'H')
                playerSprite.setAnimation("failInk");
            else
                playerSprite.setAnimation("fail");

            Util::doLater([this] { this->canMove = false; }, 100);
            Util::doLater([this] { this->reset(); }, 1500);

            return;
        }

        totalDelta += delta;
    }

    playerPosInt += totalDelta;
    ghostPosInt = playerPosInt;
    showGhost = false;
}

void GUIScene::moveGhost(Control input)
{
    auto deltas = controlToDeltas(input);

    for(auto& delta : deltas)
        ghostPosInt += delta;
}

void GUIScene::tryUndo()
{
    if (!canMove || pastPositions.empty())
    {
        LOG("Can't move your body."); // Undertale reference!!!!!!
        return;
    }

    playerPosInt = pastPositions.back();
    pastPositions.pop_back();

    LOG_INFO("Undid move!");
}

void GUIScene::reset()
{
    playerPosInt = level.start;
    canMove = true;
    playerSprite.setAnimation("default");
    flag.hidden = false;
    pastPositions.clear();

    inputQueue.clear();

    ((GUIControlsOverlay*)Outrospection::get().controlsOverlay)->setControls(level.controls);
}

bool GUIScene::controlBound(Control control)
{
    if (control == Control::NONE)
        return false; // NONE cannot be bound

    for (KeyBinding& bind : keyBinds)
    {
        if (bind.m_eye != Eye::NONE && bind.m_control == control)
            return true; // can't bind control twice!
    }

    return false;
}

void GUIScene::doControl(Eye pokedEye)
{
    Outrospection::get().scheduleWorldTick(); // do tick NOW

    for (KeyBinding& bind : keyBinds)
    {
        if (bind.m_eye == pokedEye)
        {
            inputQueue.emplace_back(bind.m_control);
        }
    }
}

void GUIScene::doGhostControl(Eye hoveredEye)
{
    //Outrospection::get().scheduleWorldTick(); // do tick NOW

    for (KeyBinding& bind : keyBinds)
    {
        if (bind.m_eye == hoveredEye)
        {
            ghostInputQueue.emplace_back(bind.m_control);
        }
    }
}

std::vector<glm::vec2> GUIScene::controlToDeltas(Control ctrl)
{
    std::vector<glm::vec2> deltas;
    switch (ctrl)
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
        LOG_ERROR("Unknown control %i", int(ctrl));
    }

    return deltas;
}
