#pragma once

// Platform detection using predefined macros
#ifdef _WIN32
    /* Windows x64/x86 */
    #ifdef _WIN64
        /* Windows x64  */
        #define PLATFORM_WINDOWS
    #else
        /* Windows x86 */
        #error "x86 not supported"
    #endif

    /* If you REALLY want to...*/
    //#define PLATFORM_XP

#elif defined(__APPLE__) || defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE == 1
        #define PLATFORM_IOS
        #error "iOS is not supported!"
    #elif TARGET_OS_MAC == 1
        #define PLATFORM_MACOS
    #else
        #error "Unknown Apple platform (is probably not supported)!"
    #endif

#elif defined(__ANDROID__)
    #define PLATFORM_ANDROID
    #error "Android is not supported!"

#elif defined(__linux__)
    #define PLATFORM_LINUX

#elif defined(__EMSCRIPTEN__)
    #define PLATFORM_WEB

#else
    #error "Unknown platform!"
#endif // End of platform detection

