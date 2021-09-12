#ifndef B_ROOM_H_INCLUDED
#define B_ROOM_H_INCLUDED

#include <vector>

#include <PGE/Graphics/Mesh.h>
#include <PGE/Math/Vector.h>
#include <PGE/File/FilePath.h>

#include "../Resources.h"
#include "../../Collision/CollisionMesh.h"

class Room {
	public:
		Room(Resources& res, const PGE::FilePath& path);
		~Room();

		void render(const PGE::Matrix4x4f& mat) const;

		const CollisionMesh& getCollisionMesh() const;

		void toggleDebug();

	private:
		std::vector<PGE::Mesh*> meshes;
		std::vector<Resources::Handle<PGE::Texture>> textures;
		Resources::Handle<PGE::Texture> debugTex;
		bool debug = false;
		PGE::Shader& roomShader;
		PGE::Shader::Constant* matrixConstant;
		CollisionMesh* collisionMesh;
};

#endif // B_ROOM_H_INCLUDED
