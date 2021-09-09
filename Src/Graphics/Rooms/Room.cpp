#include "Room.h"

#include <PGE/File/BinaryReader.h>

#include "../../Utilities/Directories.h"

using namespace PGE;

Room::Room(Resources& res, const FilePath& path) : roomShader(res.getRoomShader()) {
	matrixConstant = &roomShader.getVertexShaderConstant("worldMatrix");

	BinaryReader reader(path + ".b");

	std::vector<Vector3f> cVertices;
	std::vector<u32> cIndices;
	byte texCount = reader.read<byte>();
	meshes.reserve(texCount);
	textures.reserve(texCount);
	for (int i = 0; i < texCount; i++) {
		meshes.push_back(Mesh::create(res.getGraphics()));

		FilePath textureName = Directories::TEXTURES + reader.read<String>();
		ReferenceVector<Texture> currTexs;

		textures.push_back(res.getTexture(textureName + ".png"));
		currTexs.push_back(*textures.back());
		
		textures.push_back(res.getTexture(textureName + "_r.png", Texture::Format::R8));
		currTexs.push_back(*textures.back());
		
		textures.push_back(res.getTexture(textureName + "_n.png"));
		currTexs.push_back(*textures.back());
		
		textures.push_back(res.getTexture(textureName + "_d.png", Texture::Format::R8));
		currTexs.push_back(*textures.back());
		
		meshes.back()->setMaterial(Mesh::Material(roomShader, currTexs, Mesh::Material::Opaque::YES));

		i32 vertCount = reader.read<i32>();
		StructuredData data(roomShader.getVertexLayout(), vertCount);
		int oldCVertSize = (int)cVertices.size();
		cVertices.resize(cVertices.size() + vertCount);
		for (int j = 0; j < vertCount; j++) {
			cVertices[oldCVertSize + j] = reader.read<Vector3f>();
			data.setValue(j, "position", cVertices[oldCVertSize + j]);
			data.setValue(j, "normal", reader.read<Vector3f>());
			data.setValue(j, "tangent", reader.read<Vector3f>());
			data.setValue(j, "bitangent", reader.read<Vector3f>());
			data.setValue(j, "uv", reader.read<Vector2f>());
		}
		
		i32 primCount = reader.read<i32>();
		std::vector<u32> primitives(primCount);
		int oldCIndicesSize = (int)cIndices.size();
		cIndices.resize(cIndices.size() + primCount);
		for (int j = 0; j < primCount; j++) {
			cIndices[oldCIndicesSize + j] = (primitives[j] = reader.read<i32>()) + oldCVertSize;
		}

		meshes.back()->setGeometry(std::move(data), Mesh::PrimitiveType::TRIANGLE, std::move(primitives));
	}

	collisionMesh = new CollisionMesh(std::move(cVertices), std::move(cIndices));
}

Room::~Room() {
	delete collisionMesh;
	for (Mesh* m : meshes) { delete m; }
	for (Resources::Handle<Texture> tex : textures) { tex.drop(); }
}

void Room::render(const Matrix4x4f& mat) const {
	matrixConstant->setValue(mat);
	for (Mesh* m : meshes) { m->render(); }
}

const CollisionMesh& Room::getCollisionMesh() const {
	return *collisionMesh;
}
