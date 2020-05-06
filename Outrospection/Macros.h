#pragma once

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);   \
  void operator=(const TypeName&);

#define DISALLOW_COPY(TypeName) \
  TypeName(const TypeName&);

#define DISALLOW_ASSIGN(TypeName) \
  void operator=(const TypeName&);

// subtract from vertical velocity
#define GRAVITY -0.25

// divide velocity by this to slow down
#define FRICTION 1.35

#define SCR_WIDTH 1152
#define SCR_HEIGHT 648


// CONTROLLER

// joystick value under this means 0
#define STICK_DEADZONE 0.25
// joystick value over this means 1.0
#define STICK_LIMITZONE 0.95

// axes IDs
#define STICK_LEFT_SIDE 0
#define STICK_LEFT_UP 1
#define STICK_TRIGGER 2 // Ltrigger > deadzone, Rtrigger < -deadzone
#define STICK_RIGHT_UP 3
#define STICK_RIGHT_SIDE 4

// buttons IDs
#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_X 2
#define BUTTON_Y 3
#define BUTTON_LBUMBER 4
#define BUTTON_RBUMBER 5
#define BUTTON_BACK 6
#define BUTTON_START 7
#define BUTTON_LSTICK 8
#define BUTTON_RSTICK 9
#define BUTTON_D_UP 10
#define BUTTON_D_RIGHT 11
#define BUTTON_D_DOWN 12
#define BUTTON_D_LEFT 13


#define DEBUG true
#define VERBOSE DEBUG && true