//#include "MultiTexture.h"
//#include <fstream>
//
//MultiTexture::MultiTexture(std::string multiTextureFileName) {
//	std::ifstream multiTexFile("./res/ObjectData/Characters/" + multiTextureFileName + ".gfi"); // GO FOR IT!
//	bool isFile = false;
//
//	for (std::string line; getline(multiTexFile, line); )
//	{
//		// signature check
//		if (line.compare("GO FOR IT!") == 0) {
//			isFile = true;
//		}
//		else if (!isFile) {
//			std::cout << "ERROR: File " << "./res/ObjectData/Characters/" + multiTextureFileName + ".gfi" << " is not a GFI file!";
//			return;
//		}
//
//		int separatorIndex = line.find("|");
//		if (separatorIndex == std::string::npos) {
//			std::cout << "ERROR: Line \"" << line << "\" does not contain separator char \"|\" in file " << "./res/ObjectData/Characters/" + multiTextureFileName + ".gfi" << "!";
//			return;
//		}
//
//		if (line.length() < separatorIndex + 1) {
//			std::cout << "ERROR: Line \"" << line << "\" does not contain animation data after separator \"|\" in file " << "./res/ObjectData/Characters/" + multiTextureFileName + ".gfi" << "!";
//			return;
//		}
//		char propsChar = line.at(separatorIndex + 1);
//
//		bool bottom    = (propsChar << 0) & 1;
//		bool legs      = (propsChar << 1) & 1;
//		bool arms      = (propsChar << 2) & 1;
//		bool torso     = (propsChar << 3) & 1;
//		bool underface = (propsChar << 4) & 1;
//		bool face      = (propsChar << 5) & 1;
//		bool top       = (propsChar << 6) & 1;
//
//		// what do with this
//		std::cout << "Read date for " << line.substr(0, separatorIndex) << ": bot = " << bottom << " leg = " << legs << " arm = " << arms << " torso = " << torso << " underface = " << underface << " face = " << face << " top = " << top;
//
//	}
//
//	multiTexFile.close();
//}