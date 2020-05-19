#pragma once

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;   \
  TypeName& operator=(const TypeName&) = delete;

#define DISALLOW_COPY(TypeName) \
  TypeName(const TypeName&) = delete;

#define DISALLOW_ASSIGN(TypeName) \
  TypeName& operator=(const TypeName&) = delete;

// subtract from vertical velocity
constexpr auto GRAVITY = -0.25f;

// divide velocity by this to slow down
constexpr auto FRICTION = 1.35;

constexpr auto SCR_WIDTH = 1152;
constexpr auto SCR_HEIGHT = 648;


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
constexpr auto BUTTON_LBUMBER = 4;
constexpr auto BUTTON_RBUMBER = 5;
constexpr auto BUTTON_BACK = 6;
constexpr auto BUTTON_START = 7;
constexpr auto BUTTON_LSTICK = 8;
constexpr auto BUTTON_RSTICK = 9;
constexpr auto BUTTON_D_UP = 10;
constexpr auto BUTTON_D_RIGHT = 11;
constexpr auto BUTTON_D_DOWN = 12;
constexpr auto BUTTON_D_LEFT = 13;


constexpr auto DEBUG = true;
constexpr auto VERBOSE = DEBUG && false;