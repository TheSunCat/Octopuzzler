#pragma once
#include <sstream>
#include <fstream>

#include "Core.h"
#include "Types.h"
#include "Util.h"

const Level EMPTY_LEVEL = { "\
wwww\
w  w\
wwww", 4, {1, 1}, {2, 1}
};

inline Level readLevel(Resource& levelResource, int levelID)
{
	std::stringstream ss;
	ss << levelResource.getResourcePath() << levelID << ".opz";
	std::string levelName = ss.str();

    std::string levelData;
    std::ifstream levelFile;

    // ensure ifstream objects can throw exceptions:
    levelFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        levelFile.open(levelName);
        std::stringstream levelStream;
        levelStream << levelFile.rdbuf();
        levelFile.close();
    	
        levelData = levelStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        LOG_ERROR("Failed to read level file \"%s\"! errno %s", levelName, e.what());
        return EMPTY_LEVEL;
    }

	if(!levelData.starts_with("OCTOLEVL"))
	{
        LOG_ERROR("File magic is not OCTOLEVL");
        return EMPTY_LEVEL;
	}

    levelData = levelData.substr(8); // cut off file magic

	// TODO I don't know how to code this please send help
}
