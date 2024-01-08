#pragma once

#include <chrono>
#include <functional>
#include <iomanip>

#include "Platform.h"

#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#undef APIENTRY
#include "Windows.h"
#undef APIENTRY

// stolen from https://github.com/ikalnytskyi/termcolor/blob/master/include/termcolor/termcolor.hpp#L567, thanks!
//! Change Windows Terminal colors attribute. If some
//! parameter is `-1` then attribute won't changed.
inline void win_change_attributes(const int foreground)
{
    // yeah, i know.. it's ugly, it's windows.
    static WORD defaultAttributes = 0;

    HANDLE hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);

    // save default terminal attributes if it unsaved
    if (!defaultAttributes)
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(hTerminal, &info))
            return;
        defaultAttributes = info.wAttributes;
    }

    // restore all default settings
    if (foreground == -1)
    {
        SetConsoleTextAttribute(hTerminal, defaultAttributes);
        return;
    }

    // get current settings
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(hTerminal, &info))
        return;

    if (foreground != -1)
    {
        info.wAttributes &= ~(info.wAttributes & 0x0F);
        info.wAttributes |= static_cast<WORD>(foreground);
    }

    info.wAttributes &= ~(info.wAttributes & 0xF0);
    info.wAttributes |= static_cast<WORD>(0);

    SetConsoleTextAttribute(hTerminal, info.wAttributes);
}

#define CHANGE_COLOR(col) win_change_attributes((col) == 0 ? -1 : (col));

#elif defined(PLATFORM_WEB)
#define CHANGE_COLOR(col) // no colors on web!
#else

#define CHANGE_COLOR(col) printf("\033[%im", (col))
#endif

#include <iostream>

#define LOG(...) {auto now = std::chrono::system_clock::now(); \
    auto in_time_t = std::chrono::system_clock::to_time_t(now); \
    std::cout << std::put_time(std::localtime(&in_time_t), "[%X] ");\
    printf(__VA_ARGS__); printf("\n"); }

#ifdef _DEBUG 
#define LOG_DEBUG(...) loggerQueue.push([args=std::make_tuple(__VA_ARGS__)] { CHANGE_COLOR(35); /* set color to magenta */\
        std::apply(smart_printf{}, args); \
        CHANGE_COLOR(0);})

#define PROFILE Util::Timer timer_##__COUNTER__(__func__)
#else
#define LOG_DEBUG(...)
#define PROFILE
#endif

#define LOG_ERROR(...) CHANGE_COLOR(4); /* red error color */\
        LOG(__VA_ARGS__); \
        CHANGE_COLOR(0)

#define LOG_INFO(...) CHANGE_COLOR(34); /* green info color */\
        LOG(__VA_ARGS__); \
        CHANGE_COLOR(0)

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete;   \
    TypeName& operator=(const TypeName&) = delete;

#define DISALLOW_COPY(TypeName) \
    TypeName(const TypeName&) = delete;

#define DISALLOW_ASSIGN(TypeName) \
    TypeName& operator=(const TypeName&) = delete;

#define BIT(x) (1 << (x))

#define BIND_EVENT_FUNC(function) std::bind(&function, this, std::placeholders::_1)

#define GET_ITEM(item) Outrospection::get().itemRegistry.get(item)
#define ITEM_EXISTS(item) Outrospection::get().itemRegistry.has(item)
