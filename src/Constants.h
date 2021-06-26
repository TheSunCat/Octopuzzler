#pragma once
#include <array>
#include <GLAD/glad.h>

#include "Types.h"

// divide velocity by this to slow down
constexpr auto FRICTION = 1.35f;


// CONTROLLER

// joystick value under this means 0
constexpr auto STICK_DEADZONE = 0.25f;
// joystick value over this means 1.0
constexpr auto STICK_LIMITZONE = 0.95f;

// axes IDs
constexpr auto STICK_LEFT_SIDE = 0;
constexpr auto STICK_LEFT_UP = 1;
constexpr auto STICK_TRIGGER = 2; // Ltrigger > deadzone, Rtrigger < -deadzone
constexpr auto STICK_RIGHT_UP = 3;
constexpr auto STICK_RIGHT_SIDE = 4;

// buttons IDs
constexpr auto BUTTON_A = 0;
constexpr auto BUTTON_B = 1;
constexpr auto BUTTON_X = 2;
constexpr auto BUTTON_Y = 3;
constexpr auto BUTTON_LBUMPER = 4;
constexpr auto BUTTON_RBUMPER = 5;
constexpr auto BUTTON_BACK = 6;
constexpr auto BUTTON_START = 7;
constexpr auto BUTTON_LSTICK = 8;
constexpr auto BUTTON_RSTICK = 9;
constexpr auto BUTTON_D_UP = 10;
constexpr auto BUTTON_D_RIGHT = 11;
constexpr auto BUTTON_D_DOWN = 12;
constexpr auto BUTTON_D_LEFT = 13;

constexpr std::array<Vertex, 8> cubeVerts = { {
        // positions          
       {{ 0.5, 0.5, 0.5}}, // 000 1
       {{ 0.5, 0.5,-0.5}}, // 001 2
       {{ 0.5,-0.5, 0.5}}, // 010 3
       {{ 0.5,-0.5,-0.5}}, // 011 4
       {{-0.5,-0.5, 0.5}}, // 110 5
       {{-0.5,-0.5,-0.5}}, // 111 6
       {{-0.5, 0.5, 0.5}}, // 100 7
       {{-0.5, 0.5,-0.5}}, // 101 8
    } };

constexpr std::array<GLuint, 36> cubeIndices = {
    0, 1, 3,
    0, 2, 3,
    2, 3, 5,
    2, 4, 5,
    4, 6, 7,
    4, 5, 7,
    6, 7, 1,
    6, 0, 1,
    2, 4, 6,
    2, 0, 6,
    5, 3, 1,
    5, 7, 1,
};