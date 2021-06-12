#pragma once
#include "Event.h"

class KeyEvent : public Event
{
public:
    inline int getKeyCode() const
    {
        return keyCode;
    }

    EVENT_CLASS_CATEGORY(EventCategory::Keyboard | EventCategory::Input)
protected:
    KeyEvent(int _keyCode) : keyCode(_keyCode) {}
    
    int keyCode;
};

class KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(const int _keyCode, const int _repeatCount)
        : KeyEvent(_keyCode), repeatCount(_repeatCount) {}

    inline int getRepeatCount() const { return repeatCount; }

    EVENT_CLASS_TYPE(KeyPressed)
private:
    int repeatCount;
};

class KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(const int _keyCode)
        : KeyEvent(_keyCode) {}

    EVENT_CLASS_TYPE(KeyReleased)
};
