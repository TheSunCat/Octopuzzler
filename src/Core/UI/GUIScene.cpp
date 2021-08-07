#include "GUIScene.h"
#include <json.hpp>

#include "Outrospection.h"
#include "UIButton.h"
#include "GUIControlsOverlay.h"
#include "GUIGuide.h"

// this is the constructor (ctor for short).
// it only takes care of copying the level data to store it here for now
GUIScene::GUIScene() : GUILayer("Scene", false),
                    floor("floor", animatedTexture({ "UI/floor/", "empty" }, 8, 17, GL_NEAREST), UITransform(0, 0, 100, 100, {640, 480})),
                    ink("hole", GL_NEAREST, UITransform(0, 0, 100, 100, {640, 480})),
                    flag("flag", animatedTexture({"UI/flag/", "default"}, 16, 2, GL_NEAREST), UITransform(0, 0, 0, 0, {640, 480})),
                    background("background", animatedTexture({"UI/background/", "default"}, 8, 17, GL_NEAREST), UITransform(0, 0, 10, 10, {640, 480})),
                    playerSprite("player", animatedTexture({ "UI/player/", "default" }, 16, 2, GL_NEAREST), UITransform(0, 0, 10, 10, {640, 480})),
                    ghostSprite("ghost", animatedTexture({ "UI/ghost/", "default" }, 16, 2, GL_NEAREST), UITransform(0, 0, 10, 10, {640, 480}))

{
    handleManually = true;

    playerSprite.addAnimation("fail", simpleTexture({"UI/player/", "fail"}, GL_NEAREST));
    playerSprite.addAnimation("failInk", simpleTexture({"UI/player/", "failInk"}, GL_NEAREST));
    playerSprite.addAnimation("win", simpleTexture({"UI/player/", "win"}, GL_NEAREST));

    setLevel("", levelID);
}

void GUIScene::setLevel(const std::string& lvlName, int lvlID)
{
    // parse level
    std::string levelData = Util::readAllBytes("StageData/level" + lvlName + lvlID);
    
    nlohmann::json jason = nlohmann::json::parse(levelData); // this is a joke
    level = jason.get<Level>();                                 // please laugh
    
    if(Outrospection::get().isSpeedrun())
    {
        level.controls = "UDLR^V<>"; // unlock all controls

        // disable guides
        level.guideLeft = "default";
        level.guideRight = "default";
    }

    playerPosInt = level.start;
    ghostSprite.visible = false;

    Util::doLater([this]
    {
        this->reset();
    }, 100);
}

void GUIScene::tick()
{
    playerPos = Util::lerp(playerPos, playerPosInt, 0.2);

    if (!canMove || (canMove && inputQueue.empty() && playerPos != playerPosInt)) // player is not currently moving
    {
        if (!ghostInputQueue.empty() && !Outrospection::get().won)
            ghostSprite.visible = true; // ghost is moving
        else
            ghostSprite.visible = false; // ghost is stopped
    }
    else
    {
        ghostSprite.visible = false;
    }

    if(!ghostSprite.visible)
    {
        ghostPosInt = playerPosInt; ghostPos = ghostPosInt;
        curGhostMove = -3;
    } else
    {
        ghostPos = Util::lerp(ghostPos, ghostPosInt, 0.2);
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

    float spriteScale = 480.f / largestLength; // fill leftmost 480x480 area with actual level

    // adding 1px to prevent lines between sprites when scaled down
    playerSprite.setScale(spriteScale + 1);
    ghostSprite.setScale(spriteScale + 1);
    floor.setScale(spriteScale + 1);
    ink.setScale(spriteScale + 1);
    flag.setScale(spriteScale + 1);
    background.setScale(spriteScale + 1);

    for(int x = 0; x < largestLength + 5; x++) // dirty hack to cover further right with bg tiles
    {
        for(int y = 0; y < largestLength; y++)
        {
            float xSpritePos = x * spriteScale;
            float ySpritePos = y * spriteScale;

            background.setPosition(xSpritePos, ySpritePos);
            background.draw(spriteShader, glyphShader);

            if (x < (largestLength - rowLength) / 2 || x >= rowLength + (largestLength - rowLength) / 2 ||
                y < (largestLength - colLength) / 2 || y >= colLength + (largestLength - colLength) / 2)
                continue;  // tile not in level

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

    if (canMove) {
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

        tryMovePlayer(curInput);

        // erase it so we move to the next input
        inputQueue.erase(inputQueue.begin());
    }

    if(curGhostMove >= 0 && !ghostInputQueue.empty())
    {
        // read input
        Control curInput = ghostInputQueue[curGhostMove];

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

    auto deltas = controlToDeltas(input);

    glm::vec2 totalDelta = glm::vec2();

    glm::vec2 ghostPlayerPos = playerPosInt;

    for (int i = 0; i < deltas.size(); i++)
    {
        glm::vec2& delta = deltas[i];

        ghostPlayerPos += delta;

        if (ghostPlayerPos == level.goal && (i == deltas.size() - 1) && (inputQueue.size() == 1))
        {
            playerPosInt = ghostPlayerPos;
            LOG_INFO("You win!! :D");

            Outrospection::get().audioManager.play("Flag_Get");

            flag.visible = false;
            playerSprite.setAnimation("win");
            canMove = false;

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
                    playerSprite.setAnimation("default");

                    setLevel("", this->levelID);

                    LOG_INFO("Advancing to level %i...", this->levelID);
                }
            }, 1000);
            return;
        }

        int intGhostPosition = ghostPlayerPos.x + (ghostPlayerPos.y * level.rowLength);
        char tile = level.data[intGhostPosition];

        if (tile != ' ' && !((tile == 'H' || tile == 'G') && i == 0 && deltas.size() == 2))
        {
            LOG_INFO("Player died!");
            canMove = false;
            playerPosInt = ghostPlayerPos;

            if(tile == 'H')
                playerSprite.setAnimation("failInk");
            else
                playerSprite.setAnimation("fail");

            Util::doLater([this] { this->canMove = false; }, 100);
            Util::doLater([this] {
                if(!Outrospection::get().isSpeedrun())
                    this->reset();
                else {
                    LOG_INFO("Resetting entire game...");
                    levelID = 0;
                    setLevel("", levelID);
                }
            }, 1500);

            return;
        }

        totalDelta += delta;
    }

    Outrospection::get().audioManager.play("Movement");

    playerPosInt += totalDelta;
    ghostPosInt = playerPosInt;
    ghostSprite.visible = false;
    curGhostMove = -3;
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
    flag.visible = true;

    ghostSprite.visible = false;
    ghostPosInt = playerPosInt; ghostPos = ghostPosInt;
    ghostInputQueue.clear();

    pastPositions.clear();
    inputQueue.clear();

    ((GUIControlsOverlay*)Outrospection::get().controlsOverlay)->setControls(level.controls);

    ((GUIGuide*)Outrospection::get().guideOverlay)->setRightGuide(level.guideRight);
    ((GUIGuide*)Outrospection::get().guideOverlay)->setLeftGuide(level.guideLeft);
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
    pastPositions.emplace_back(playerPosInt);

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
