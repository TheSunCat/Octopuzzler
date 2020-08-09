#pragma once
#include <list>
#include <map>
#include <typeindex>

#include "Events/EventHandlerFunction.h"

typedef std::list<HandlerFunctionBase*> HandlerList;

class EventBus
{
public:
    template <typename EventType>
    void publish(EventType* event)
    {
        HandlerList* handlers = subscribers[typeid(EventType)];

        if (handlers == nullptr)
        {
            return;
        }

        for (auto& handler : *handlers)
        {
            if (handler != nullptr)
            {
                handler->exec(event);
            }
        }
    }

    template <class T, class EventType>
    void subscribe(T* instance, void (T::* memberFunction)(EventType*))
    {
        HandlerList* handlers = subscribers[typeid(EventType)];

        // first time initialization
        if (handlers == nullptr)
        {
            handlers = new HandlerList();
            subscribers[typeid(EventType)] = handlers;
        }

        handlers->push_back(new MemberFunctionHandler<T, EventType>(instance, memberFunction));
    }

private:
    std::map<std::type_index, HandlerList*> subscribers;
};
