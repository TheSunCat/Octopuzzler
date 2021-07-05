#include "Outrospection.h"

#ifdef PLATFORM_WINDOWS
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

#ifdef _DEBUG
    if (AllocConsole()) {
#else
    if (AttachConsole(ATTACH_PARENT_PROCESS)) { // set up console output (if there is a console to attach to)
#endif
        FILE* empty;
        freopen_s(&empty, "CONOUT$", "w", stdout);
        freopen_s(&empty, "CONOUT$", "w", stderr);
    }

    //std::cout << "test";
#else
int main() {
#endif

    // check if we can read the filesystem
    bool canReadFiles = Util::fileExists("ShaderData/crt.vert");
    if(!canReadFiles)
    {
        std::cout << "Can't access \"res\" folder! Make sure you:\n- run the game from its directory\n- see the \"res\" folder next to the game\n- have unzipped it fully" << std::endl;
        std::cin.get();
        return -1;
    }

    // run the game!
    Outrospection().run();
    return 0;
}
