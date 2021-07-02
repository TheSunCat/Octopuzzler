#include "Outrospection.h"

int main()
{
    // check if we can read the filesystem
    bool canReadFiles = Util::fileExists("ShaderData/CRT.vert");
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
