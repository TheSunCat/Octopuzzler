cmake_minimum_required(VERSION 3.18)

# Change project name here
set(PROJECT_NAME "Octopuzzler" CACHE STRING "Project name")

set(GL_COMPAT ON CACHE BOOL "Run with OpenGL 3.3 (and support legacy fixed function as well)")

# enable leak checking
set(LEAK_CHECK ON CACHE BOOL "Check memory leaks (must be OFF for debugging)")

# Web options
set(ASPECT_RATIO "16/9" CACHE STRING "Aspect ratio")
set(CLICK_TO_START ON CACHE BOOL "A click is required to activate sound on web")

# --------------------------------------
# no need to change anything after this!
# --------------------------------------

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "-g -fPIC")
endif()

# enable C++20
set(CMAKE_CXX_STANDARD 20)

# enable leak checking
if(LEAK_CHECK AND NOT EMSCRIPTEN)
    add_compile_options(-fno-omit-frame-pointer -fsanitize=address)
    add_link_options(-fno-omit-frame-pointer -fsanitize=address)
endif()

# for LSP support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("${PROJECT_NAME}")

file(GLOB_RECURSE DIR_SRC src/*.cpp)
file(GLOB_RECURSE SOLOUD_SRC lib/soloud/*.cpp) # bit of a hack but eh
file(GLOB_RECURSE SOLOUD_SRC_C lib/soloud/*.c) # bit of a hack but eh

add_executable("${PROJECT_NAME}" ${DIR_SRC} ${SOLOUD_SRC} ${SOLOUD_SRC_C})

# soloud backend
if(EMSCRIPTEN)
    # miniaudio has bugs on emscripten, so we have to use SDL :(
    target_compile_definitions(${PROJECT_NAME} PRIVATE WITH_SDL2_STATIC)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE WITH_MINIAUDIO)
endif()

if(EMSCRIPTEN)
    include(Emscripten)

else()
    find_package(glfw3 REQUIRED)
    target_link_libraries(${PROJECT_NAME} glfw)

    find_package(Freetype REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES})
    include_directories(${PROJECT_NAME} ${FREETYPE_INCLUDE_DIRS})

    find_package(OpenGL)
    include_directories(${PROJECT_NAME} ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
endif()

add_subdirectory(lib/glad/)
target_link_libraries(${PROJECT_NAME} glad)

# include directories
include_directories(src)
include_directories(lib/soloud/include)

# disable warnings for external libraries
file(GLOB_RECURSE EXTERNAL_DIR "lib/**")
set_source_files_properties(
  ${EXTERNAL_DIR}
  PROPERTIES
  COMPILE_FLAGS "-w"
)

if(GL_COMPAT)
    target_compile_definitions(${PROJECT_NAME} PUBLIC GL_COMPAT)
endif()

# symlink resources folder on supported platforms (sorry, Microsoft Windows!)
if(NOT WIN32)
    add_custom_command(TARGET "${PROJECT_NAME}" PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
else()
    MicrosoftWindows_AddCompileCommands()
endif()
