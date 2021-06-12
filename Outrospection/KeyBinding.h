#pragma once
#include "Types.h"

class KeyBinding
{
public:
    KeyBinding(Eye eye, Control control);

    Eye m_eye;
    Control m_control;
};
