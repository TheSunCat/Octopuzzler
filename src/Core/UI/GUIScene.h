#pragma once
#include "Core/UI/GUILayer.h"
#include "Core/UI/UIComponent.h"
#include "KeyBinding.h"

class GUIScene : public GUILayer
{
public:
    GUIScene();
    void setLevel(int lvlID);

    void tick() override;

    void draw() const override;

    void worldTick();

    void tryUndo();
    void reset();

    bool controlBound(Control control);
    void doControl(Eye pokedEye);
    void doGhostControl(Eye hoveredEye);

    std::vector<KeyBinding> keyBinds{};
    std::vector<Control> inputQueue;

    Level level{};
    glm::vec2 playerPosInt{}; // actual position on grid
    mutable bool canMove = true;
    mutable std::vector<glm::vec2> pastPositions;

    glm::vec2 ghostPosInt{}; // actual position on grid
    int curGhostMove = -1;
    std::vector<Control> ghostInputQueue;

    // TODO make this private and control visibility w/ a function
    mutable UIComponent ghostSprite;
private:
    void tryMovePlayer(Control input);
    void moveGhost(Control input);

    static std::vector<glm::vec2> controlToDeltas(Control ctrl);

    glm::vec2 playerPos{}; // interpolates player between grid spots
    glm::vec2 ghostPos{};  // interpolates ghost between grid spots


    mutable UIComponent floor;
    mutable UIComponent ink;
    mutable UIComponent flag;
    mutable UIComponent background;

    mutable UIComponent playerSprite;

    mutable UIComponent levelProgress;

    std::vector<std::string> levelFiles;
    mutable int levelID = 0;
};
