#pragma once

#include "Event.h"

class MouseMovedEvent : public Event
{
public:
    MouseMovedEvent(const float x, const float y)
        : mouseX(x), mouseY(y) {}

    inline float getX() const { return mouseX; }
    inline float getY() const { return mouseY; }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input)

private:
    float mouseX, mouseY;
};

class MouseScrolledEvent : public Event
{
public:
    MouseScrolledEvent(float _xOffset, float _yOffset)
        : xOffset(_xOffset), yOffset(_yOffset) {}

    inline float getX() const { return xOffset; }
    inline float getY() const { return yOffset; }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input)
private:
    float xOffset, yOffset;
};

class MouseButtonEvent : public Event
{
public:
    inline int getMouseButton() const { return button; }

    EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input)
protected:
    MouseButtonEvent(const int _button)
        : button(_button) {}

    int button;
};

class MouseButtonPressedEvent : public MouseButtonEvent
{
public:
    MouseButtonPressedEvent(const int _button)
        : MouseButtonEvent(_button) {}

    EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
    MouseButtonReleasedEvent(const int _button)
        : MouseButtonEvent(_button) {}

    EVENT_CLASS_TYPE(MouseButtonReleased)
};
