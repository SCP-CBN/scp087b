#ifndef B_ROOM_H_INCLUDED
#define B_ROOM_H_INCLUDED

#include <vector>

#include <PGE/Graphics/Mesh.h>
#include <PGE/Math/Vector.h>
#include <PGE/File/FilePath.h>

#include "../Resources.h"

class Room {
	public:
		Room(Resources& res, const PGE::FilePath& path);
		~Room();

		void render(const PGE::Matrix4x4f& mat) const;

		PGE::Shader& getShader() const;

	private:
		std::vector<PGE::Mesh*> meshes;
		std::vector<Resources::Handle<PGE::Texture>> textures;
		Resources::Handle<PGE::Shader> roomShader;
		PGE::Shader::Constant* matrixConstant;
};

#endif // B_ROOM_H_INCLUDED
