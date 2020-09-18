#pragma once

class Event;

// This is the interface for MemberFunctionHandler that each specialization will use
class HandlerFunctionBase
{
public:
    // Call the member function
    void exec(Event* event)
    {
        call(event);
    }

private:
    // Implemented by MemberFunctionHandler
    virtual void call(Event* event) = 0;
};

template <class T, class EventType>
class MemberFunctionHandler : public HandlerFunctionBase
{
public:
    typedef void (T::* MemberFunction)(EventType*);

    MemberFunctionHandler(T* instance, MemberFunction memberFunction) : instance{instance},
                                                                        memberFunction{memberFunction}
    {
    };

    void call(Event* event) override
    {
        // Cast event to the correct type and call member function
        (instance->*memberFunction)(static_cast<EventType*>(event));
    }

private:
    // Pointer to class instance
    T* instance;

    // Pointer to member function
    MemberFunction memberFunction;
};
