#ifndef B_ROOM_H_INCLUDED
#define B_ROOM_H_INCLUDED

#include <vector>
#include <array>

#include <PGE/Graphics/Mesh.h>
#include <PGE/Math/Vector.h>
#include <PGE/File/FilePath.h>
#include <PGE/File/BinaryReader.h>

#include "../Resources.h"
#include "../../Collision/CollisionMesh.h"

class Room {
	public:
		enum MeshType {
			CEILING = 0,
			FLOOR = 1,
			INNER_WALL = 2,
			OUTER_WALL = 3,
		};

		struct RenderInfo {
			std::array<PGE::Vector2f, 4> offsets;
			float rotation;
		};
	
		Room(Resources& res, const PGE::FilePath& path);
		~Room();

		void render(const PGE::Matrix4x4f& mat, const RenderInfo& info) const;

		const CollisionMesh& getCollisionMesh() const;

		const PGE::Vector2f& getUvOffset(MeshType type) const;

		void toggleDebug();

	private:
		std::array<PGE::Vector2f, 4> uvOffsets;
	
		std::array<PGE::Mesh*, 4> iMeshes;
		std::vector<PGE::Mesh*> otherMeshes;

		std::vector<Resources::Handle<PGE::Texture>> textures;
		Resources::Handle<PGE::Texture> debugTex;
		bool debug = false;
		
		PGE::Shader& roomShader;
		PGE::Shader::Constant* matrixConstant;
		PGE::Shader::Constant* uvOffConstant;
		PGE::Shader::Constant* uvRotConstant;
		
		CollisionMesh* collisionMesh;

		const PGE::ReferenceVector<PGE::Texture> readMaterial(Resources& res, const PGE::FilePath& tex);
		void readMesh(PGE::BinaryReader& reader, PGE::Mesh& mesh, std::vector<PGE::Vector3f>& cVertices, std::vector<PGE::u32>& cIndices);
};

#endif // B_ROOM_H_INCLUDED
