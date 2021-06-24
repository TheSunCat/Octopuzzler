#include "GUIScene.h"

#include "Outrospection.h"
#include "UIButton.h"
#include "Core/UI/GUIControlsOverlay.h"
#include <json.hpp>

// this is the constructor (ctor for short).
// it only takes care of copying the level data to store it here for now
GUIScene::GUIScene() : GUILayer("Scene", false),
                       floor("platform", GL_NEAREST, 0, 0, 0.1, 0.1),
                       ink("hole", GL_NEAREST, 0, 0, 0.1, 0.1),
                       flag("flag", animatedTexture({"UI/flag/", "default"}, 10, 2, GL_NEAREST), {0, 0}, {0, 0}),
                       playerSprite("player/default", GL_NEAREST, 0, 0, 0.1, 0.1)

{
    handleManually = true;

    playerSprite.addAnimation("die", simpleTexture({"UI/player/", "sad"}, GL_NEAREST));
    playerSprite.addAnimation("win", simpleTexture({"UI/player/", "happy"}, GL_NEAREST));

    setLevel("", 0);
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

    floor.tick();
    ink.tick();

    playerSprite.tick();
}

void GUIScene::draw() const
{
    if (Outrospection::get().won)
        return;

    Shader& spriteShader = Outrospection::get().spriteShader;
    Shader& glyphShader = Outrospection::get().glyphShader;

    int rowLength = level.rowLength;
    int colLength = int(level.data.size() / level.rowLength);

    int largestLength = std::max(rowLength, colLength);

    float spriteScale = 4.f / largestLength;

    playerSprite.setScale(spriteScale, spriteScale);
    floor.setScale(spriteScale, spriteScale);
    ink.setScale(spriteScale, spriteScale);
    flag.setScale(spriteScale, spriteScale);

    for (int i = 0; i < level.data.length(); i++)
    {
        char tile = level.data[i];

        int xPos = i % rowLength;
        int yPos = i / rowLength;

        float xSpritePos = (xPos + (largestLength - rowLength) / 2.f) * spriteScale;
        float ySpritePos = (yPos + (largestLength - colLength) / 2.f) * spriteScale;

        switch (tile)
        {
        case ' ': // floor
            floor.setPosition(xSpritePos, ySpritePos);
            floor.draw(spriteShader, glyphShader);
            break;

        case 'w': // wall
            break;

        case 'H': // ink
            floor.setPosition(xSpritePos, ySpritePos);
            floor.draw(spriteShader, glyphShader);

            ink.setPosition(xSpritePos, ySpritePos);
            ink.draw(spriteShader, glyphShader);
            break;
        }
    }

    float xPlayerPos = (playerPos.x + (largestLength - rowLength) / 2.f) * spriteScale;
    float yPlayerPos = (playerPos.y + (largestLength - colLength) / 2.f) * spriteScale;

    playerSprite.setPosition(xPlayerPos, yPlayerPos);
    playerSprite.draw(spriteShader, glyphShader);

    float xFlagPos = (level.goal.x + (largestLength - rowLength) / 2.f) * spriteScale;
    float yFlagPos = (level.goal.y + (largestLength - colLength) / 2.f) * spriteScale;

    flag.setPosition(xFlagPos, yFlagPos);
    flag.draw(spriteShader, glyphShader);
}

void GUIScene::tryMovePlayer(Control input)
{
    if (!canMove)
    {
        LOG("Can't move your body."); // Undertale reference!!!!!!
        return;
    }

    pastPositions.emplace_back(playerPosInt);

    std::vector<glm::vec2> deltas;
    switch (input)
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
        LOG_ERROR("Unknown control %i", int(input));
    }

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
            playerSprite.setAnimation("die");

            Util::doLater([this] { this->canMove = false; }, 100);
            Util::doLater([this] { this->reset(); }, 1500);

            //LOG_INFO("TODO show death screen"); // TODO show death screen
            return;
        }

        totalDelta += delta;
    }


    playerPosInt += totalDelta;
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

    Outrospection::get().inputQueue.clear();

    ((GUIControlsOverlay*)Outrospection::get().controlsOverlay)->setControls(level.controls);
}
