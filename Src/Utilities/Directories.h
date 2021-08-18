#ifndef B_DIRECTORIES_H_INCLUDED
#define B_DIRECTORIES_H_INCLUDED

#include <PGE/File/FilePath.h>

namespace Directories {
	const PGE::FilePath GFX = PGE::FilePath::fromStr("GFX/");

	const PGE::FilePath ROOMS = GFX + "Rooms/";
	const PGE::FilePath SHADERS = GFX + "Shaders/";
	const PGE::FilePath TEXTURES = GFX + "Textures/";
}

#endif // B_DIRECTORIES_H_INCLUDED
