#include "Room.h"

#include <PGE/Graphics/Material.h>

#include "../../Utilities/Directories.h"

using namespace PGE;

constexpr int TEXTURES_PER_MATERIAL = 4;

Material* Room::readMaterial(Resources& res, const FilePath& tex) {
	ReferenceVector<Texture> texs; texs.reserve(TEXTURES_PER_MATERIAL);
	textures.push_back(res.getTexture(tex + ".png"));
	texs.push_back(*textures.back());

	textures.push_back(res.getTexture(tex + "_r.png", Texture::Format::R8));
	texs.push_back(*textures.back());

	textures.push_back(res.getTexture(tex + "_n.ktx2", Texture::CompressedFormat::BC5));
	texs.push_back(*textures.back());

	textures.push_back(res.getTexture(tex + "_d.png", Texture::Format::R8));
	texs.push_back(*textures.back());

	materials.emplace_back(Material::create(res.getGraphics(), res.getRoomShader(), texs, Material::Opaque::YES));
	return materials.back();
}

void Room::readMesh(BinaryReader& reader, Mesh& mesh, std::vector<Vector3f>& cVertices, std::vector<u32>& cIndices) {
	i32 vertCount = reader.read<i32>();
	StructuredData data(resources.getRoomShader().getVertexLayout(), vertCount);
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

	mesh.setGeometry(std::move(data), Mesh::PrimitiveType::TRIANGLE, std::move(primitives));
}

Room::Room(Resources& res, const FilePath& path) : resources(res) {
	Shader& roomShader = resources.getRoomShader();
	matrixConstant = &roomShader.getVertexShaderConstant("worldMatrix");
	uvOffConstant = &roomShader.getVertexShaderConstant("uvOff");
	uvRotConstant = &roomShader.getVertexShaderConstant("uvRot");

	materials.reserve(2);
	textures.reserve(2 * TEXTURES_PER_MATERIAL);
	readMaterial(res, Directories::TEXTURES + "concretefloor");
	readMaterial(res, Directories::TEXTURES + "brickwall");

	BinaryReader reader(path + ".b");

	std::vector<Vector3f> cVertices;
	std::vector<u32> cIndices;

	for (int i = 0; i < 4; i++) {
		byte index = reader.read<byte>();

		iMeshes[index] = Mesh::create(res.getGraphics());
		readMesh(reader, *iMeshes[index], cVertices, cIndices);
		uvOffsets[index] = reader.read<Vector2f>();

		iMeshes[index]->setMaterial(materials[index / 2]);
	}

	byte texCount = reader.read<byte>();
	materials.reserve(materials.size() + texCount);
	textures.reserve(textures.size() + texCount * TEXTURES_PER_MATERIAL);
	otherMeshes.reserve(texCount * TEXTURES_PER_MATERIAL);
	for (int i = 0; i < texCount; i++) {
		otherMeshes.push_back(Mesh::create(res.getGraphics()));

		FilePath textureName = Directories::TEXTURES + reader.read<String>();
		otherMeshes.back()->setMaterial(readMaterial(res, textureName));
		readMesh(reader, *otherMeshes.back(), cVertices, cIndices);
	}

	collisionMesh = new CollisionMesh(std::move(cVertices), std::move(cIndices));

	debugTex = res.getTexture(Directories::TEXTURES + "check.png");
}

Room::~Room() {
	debugTex.drop();
	delete collisionMesh;
	for (Mesh* m : otherMeshes) { delete m; }
	for (Mesh* m : iMeshes) { delete m; }
	for (Material* m : materials) { delete m; }
	for (Resources::Handle<Texture> tex : textures) { tex.drop(); }
}

void Room::render(const Matrix4x4f& mat, const RenderInfo& info) const {
	matrixConstant->setValue(mat);

	uvRotConstant->setValue(info.rotation);
	for (int i = 0; i < 4; i++) {
		if (i == 2) { uvRotConstant->setValue(0.f); }
		uvOffConstant->setValue(info.offsets[i]);
		iMeshes[i]->render();
	}
	
	uvOffConstant->setValue(Vector2f());
	for (Mesh* m : otherMeshes) { m->render(); }
}

const CollisionMesh& Room::getCollisionMesh() const {
	return *collisionMesh;
}

const Vector2f& Room::getUvOffset(MeshType type) const {
	return uvOffsets[type];
}

void Room::toggleDebug() {
	// Add the debug materials.
	if (textures.size() / TEXTURES_PER_MATERIAL == materials.size()) {
		materials.reserve(materials.size() + 2);
		Shader& roomShader = resources.getRoomShader();
		for (int i = 0; i < 2; i++) {
			ReferenceVector<Texture> currTexs; currTexs.reserve(TEXTURES_PER_MATERIAL);
			currTexs.push_back(*debugTex);
			for (int j = 1; j < TEXTURES_PER_MATERIAL; j++) {
				currTexs.push_back(*textures[i * TEXTURES_PER_MATERIAL + j]);
			}
			materials.push_back(Material::create(resources.getGraphics(), roomShader, currTexs, Material::Opaque::YES));
		}
	}

	debug =! debug;
	int start = debug ? materials.size() - 2 : 0;
	for (int i = 0; i < iMeshes.size(); i++) {
		iMeshes[i]->setMaterial(materials[start + i / 2]);
	}
}
