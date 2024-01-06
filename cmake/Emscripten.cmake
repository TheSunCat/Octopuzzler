include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 5c46b9c07008ae65cb81ab79cd677ecc1934b903
    GIT_PROGRESS TRUE
)

# Populate it for building
FetchContent_MakeAvailable(glm)

target_link_libraries(${PROJECT_NAME} glm::glm)
include_directories(${glm_SOURCE_DIR})

if(GL_COMPAT)
    target_link_libraries(${PROJECT_NAME} -sWASM=1 -sFULL_ES3=1 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2)
else()
    # we're using legacy fixed function GL2
    target_link_libraries(${PROJECT_NAME} -sLEGACY_GL_EMULATION=1 -sGL_UNSAFE_OPTS=1)
endif()

target_compile_options(${PROJECT_NAME} PUBLIC -gsource-map -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sUSE_SDL_MIXER=2 -g -Wno-switch)
target_link_libraries(${PROJECT_NAME} -sERROR_ON_UNDEFINED_SYMBOLS=0 -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sUSE_SDL_MIXER=2)

SET(CMAKE_EXECUTABLE_SUFFIX ".html")

# modify shell_minimal.html to make canvas fill the screen
file(READ ${EMSCRIPTEN_ROOT_PATH}/src/shell_minimal.html EMSCRIPTEN_SHELL_HTML)
string(FIND "${EMSCRIPTEN_SHELL_HTML}" "<style>" HAS_STYLE)
if (${HAS_STYLE} EQUAL -1)
    message(WARNING "<style> not found in shell_minimal.html, copying as-is")
else()
    string(CONCAT STYLE_REPLACEMENT "<meta name=\"viewport\" content=\"width=device-width,user-scalable=no,viewport-fit=cover\">\n"
        "    <meta name=\"apple-mobile-web-app-capable\" content=\"yes\">\n"
        "    <style>\n"
        "      body, html { border: 0px none; padding: 0px; margin: 0px; width: 100%; height: 100%; overflow: hidden; position: fixed; }\n"
        "      canvas.emscripten {"
        "          image-rendering: crisp-edges;"
        "          image-rendering: pixelated;"
        "          aspect-ratio: ${ASPECT_RATIO};"
        "          max-height: 100vh;"
        "          max-width: 100vw;"
        "          width: min(100vw, calc(100vh * calc(${ASPECT_RATIO})));"
        "          height: min(100vh, calc(100vw / calc(${ASPECT_RATIO})));"
        "          overflow: hidden;"
        "          display: block;"
        "          position: absolute;"
        "          top: 0; bottom: 0; left: 0; right: 0;"
        "          margin: auto;"
        "      }\n"
        "      .emscripten_border { width: 100%; height: 100%; border: 0px none !important;}\n"
        "      hr { display: none; }\n"
        "      div[id=\"overlay\"] {"
        "          position: absolute;"
        "          top: 0; left: 0;"
        "          width: 100%; height: 100%;"
        "          background-color: rgba(0, 0, 0, 0.5);"
        "          z-index: 9999;"
        "          display: flex;"
        "          justify-content: center;"
        "          align-items: center;"
        "          cursor: pointer;"
        "          text-shadow: 0 0 5px rgba(0, 0, 0, 0.8);"
        "     }\n"
        "     div[id=\"overlay\"] p {"
        "          background-color: #1E1E2E;"
        "          color: white;"
        "          font-size: 24px;"
        "          padding: 10px;"
        "          border-radius: 4px;"
        "          box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);"
        "     }\n"
    )
    string(REPLACE "<style>" "${STYLE_REPLACEMENT}" EMSCRIPTEN_SHELL_HTML "${EMSCRIPTEN_SHELL_HTML}")

    if(CLICK_TO_START)
        string(CONCAT BODY_REPLACEMENT "
<body>
    <div id=\"overlay\">
        <p>Click to start game</p>
    </div>

    <script>
      window.addEventListener('load', function() {
        // disable events until clicked\n
        let eat = function(event){
            event.stopImmediatePropagation();
        };

        window.addEventListener('keydown', eat, true);
        window.addEventListener('keyup', eat, true);

        var overlay = document.getElementById('overlay');
        overlay.addEventListener('click', function() {
          overlay.style.display = 'none';
          window.removeEventListener('keydown', eat, true);
          window.removeEventListener('keyup', eat, true);
        });
      });
    </script>")
        string(REPLACE "<body>" "${BODY_REPLACEMENT}" EMSCRIPTEN_SHELL_HTML "${EMSCRIPTEN_SHELL_HTML}")
    endif()

endif()
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/shell.html.in "${EMSCRIPTEN_SHELL_HTML}")


set_target_properties("${PROJECT_NAME}" PROPERTIES LINK_FLAGS "-s ASSERTIONS -s GL_DEBUG -s DEMANGLE_SUPPORT=1 -s OFFSCREEN_FRAMEBUFFER=1 -s ALLOW_MEMORY_GROWTH -s EXIT_RUNTIME=1 -s SDL2_IMAGE_FORMATS='[\"png\"]' --preload-file \"${CMAKE_SOURCE_DIR}/resources\"@resources --shell-file \"${CMAKE_CURRENT_BINARY_DIR}/shell.html.in\"")
