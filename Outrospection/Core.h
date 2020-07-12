#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
    #ifdef _WIN64
	    /* Windows x64  */
        #define PLATFORM_WINDOWS
    #else
	    /* Windows x86 */
        #error "no using x86 in the halls"
    #endif

#elif defined(__APPLE__) || defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR == 1
        #error "IOS simulator is not supported!"
    #elif TARGET_OS_IPHONE == 1
        #define PLATFORM_IOS
        #error "IOS is not supported!"
    #elif TARGET_OS_MAC == 1
        #define PLATFORM_MACOS
        #error "MacOS is not supported!"
    #else
        #error "Unknown Apple platform!"
    #endif

#elif defined(__ANDROID__)
    #define PLATFORM_ANDROID
    #error "Android is not supported!"

#elif defined(__linux__)
    #define PLATFORM_LINUX
    #error "Linux is not supported!"

#else
    #error "Unknown platform!"
#endif // End of platform detection

#define DEBUG

#ifdef PLATFORM_WINDOWS
#include "Windows.h"

// stole from https://github.com/ikalnytskyi/termcolor/blob/master/include/termcolor/termcolor.hpp#L567, thanks!
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
#else
#define CHANGE_COLOR(col) std::cout << "\033[" << (col) << 'm'
#endif

#define LOG(...) {\
    const std::time_t now = std::time(nullptr); \
    std::cout << "[" << (now / (60 * 24)) % 24 << ":" << (now / 60) % 60 << "::" << now % 60 << "] "; \
    printf(__VA_ARGS__); putchar('\n');}

#ifdef DEBUG
#define LOG_DEBUG(...) {\
    CHANGE_COLOR(35); /* set color to magenta */ \
    LOG(__VA_ARGS__); \
    CHANGE_COLOR(0);}
#else
#define LOG_DEBUG(...)
#endif

#define LOG_ERROR(...) {\
    CHANGE_COLOR(4); \
    LOG(__VA_ARGS__); \
    CHANGE_COLOR(0);}

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;   \
  TypeName& operator=(const TypeName&) = delete;

#define DISALLOW_COPY(TypeName) \
  TypeName(const TypeName&) = delete;

#define DISALLOW_ASSIGN(TypeName) \
  TypeName& operator=(const TypeName&) = delete;

#define BIT(x) (1 << (x))

#define BIND_EVENT_FUNC(function) std::bind(&function, this, std::placeholders::_1)