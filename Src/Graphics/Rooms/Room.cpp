#include "Room.h"

#include <PGE/File/BinaryReader.h>

using namespace PGE;

static const inline FilePath TEX_DIR = FilePath::fromStr("GFX/Map/");

Room::Room(Resources& res, const FilePath& path) {
	roomShader = res.getShader(FilePath::fromStr("GFX/Shaders/Room/"), true);
	matrixConstant = &roomShader->getVertexShaderConstant("worldMatrix");

	BinaryReader reader(path);

	std::vector<Vector3f> cVertices;
	std::vector<u32> cIndices;
	byte texCount = reader.read<byte>();
	meshes.resize(texCount);
	textures.resize(texCount);
	for (int i = 0; i < texCount; i++) {
		meshes[i] = Mesh::create(res.getGraphics());

		textures[i] = res.getTexture(TEX_DIR + reader.read<String>() + ".png");
		meshes[i]->setMaterial(Mesh::Material(*roomShader, *textures[i], Mesh::Material::Opaque::YES));

		i32 vertCount = reader.read<i32>();
		StructuredData data(roomShader->getVertexLayout(), vertCount);
		int oldCVertSize = cVertices.size();
		cVertices.resize(cVertices.size() + vertCount);
		for (int j = 0; j < vertCount; j++) {
			cVertices[oldCVertSize + j] = reader.read<Vector3f>();
			data.setValue(j, "position", cVertices[oldCVertSize + j]);
			data.setValue(j, "uv", reader.read<Vector2f>());
		}

		i32 primCount = reader.read<i32>();
		std::vector<u32> primitives(primCount);
		int oldCIndicesSize = cIndices.size();
		cIndices.resize(cIndices.size() + primCount);
		for (int j = 0; j < primCount; j++) {
			cIndices[oldCIndicesSize + j] = (primitives[j] = reader.read<i32>()) + oldCVertSize;
		}

		meshes[i]->setGeometry(std::move(data), Mesh::PrimitiveType::TRIANGLE, std::move(primitives));
	}

	collisionMesh = new CollisionMesh(std::move(cVertices), std::move(cIndices));
}

Room::~Room() {
	delete collisionMesh;
	for (Mesh* m : meshes) { delete m; }
	for (Resources::Handle<Texture> tex : textures) { tex.drop(); }
	roomShader.drop();
}

void Room::render(const Matrix4x4f& mat) const {
	matrixConstant->setValue(mat);
	for (Mesh* m : meshes) { m->render(); }
}

Shader& Room::getShader() const {
	return *roomShader;
}
