#include "Room.h"

#include "../../Utilities/Directories.h"

using namespace PGE;

constexpr int TEXTURES_PER_MATERIAL = 4;

void Room::readMesh(BinaryReader& reader, Mesh& mesh, std::vector<Vector3f>& cVertices, std::vector<u32>& cIndices) {
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

	mesh.setGeometry(std::move(data), Mesh::PrimitiveType::TRIANGLE, std::move(primitives));
}

Room::Room(Resources& res, const FilePath& path) : roomShader(res.getRoomShader()) {
	matrixConstant = &roomShader.getVertexShaderConstant("worldMatrix");
	uvOffConstant = &roomShader.getVertexShaderConstant("uvOff");
	uvRotConstant = &roomShader.getVertexShaderConstant("uvRot");

	BinaryReader reader(path + ".b");

	std::vector<Vector3f> cVertices;
	std::vector<u32> cIndices;

	for (int i = 0; i < 4; i++) {
		byte index = reader.read<byte>();

		iMeshes[index] = Mesh::create(res.getGraphics());
		readMesh(reader, *iMeshes[index], cVertices, cIndices);
		uvOffsets[index] = reader.read<Vector2f>();

		ReferenceVector<Texture> currTexs; currTexs.reserve(TEXTURES_PER_MATERIAL);

		FilePath textureName = Directories::TEXTURES + (index < 2 ? "concretefloor" : "brickwall");
		textures.push_back(res.getTexture(textureName + ".png"));
		currTexs.push_back(*textures.back());

		textures.push_back(res.getTexture(textureName + "_r.png", Texture::Format::R8));
		currTexs.push_back(*textures.back());

		textures.push_back(res.getTexture(textureName + "_n.png"));
		currTexs.push_back(*textures.back());

		textures.push_back(res.getTexture(textureName + "_d.png", Texture::Format::R8));
		currTexs.push_back(*textures.back());

		iMeshes[index]->setMaterial(Mesh::Material(roomShader, currTexs, Mesh::Material::Opaque::YES));
	}

	byte texCount = reader.read<byte>();
	otherMeshes.reserve(texCount * TEXTURES_PER_MATERIAL);
	textures.reserve(texCount * TEXTURES_PER_MATERIAL);
	for (int i = 0; i < texCount; i++) {
		otherMeshes.push_back(Mesh::create(res.getGraphics()));

		FilePath textureName = Directories::TEXTURES + reader.read<String>();
		ReferenceVector<Texture> currTexs; currTexs.reserve(TEXTURES_PER_MATERIAL);

		textures.push_back(res.getTexture(textureName + ".png"));
		currTexs.push_back(*textures.back());
		
		textures.push_back(res.getTexture(textureName + "_r.png", Texture::Format::R8));
		currTexs.push_back(*textures.back());
		
		textures.push_back(res.getTexture(textureName + "_n.png"));
		currTexs.push_back(*textures.back());
		
		textures.push_back(res.getTexture(textureName + "_d.png", Texture::Format::R8));
		currTexs.push_back(*textures.back());
		
		otherMeshes.back()->setMaterial(Mesh::Material(roomShader, currTexs, Mesh::Material::Opaque::YES));
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
	for (int i = 0; i < iMeshes.size(); i++) {
		ReferenceVector<Texture> currTexs; currTexs.reserve(TEXTURES_PER_MATERIAL);
		int j;
		if (debug) {
			j = 0;
		} else {
			currTexs.push_back(*debugTex);
			j = 1;
		}
		for (; j < TEXTURES_PER_MATERIAL; j++) {
			currTexs.push_back(*textures[i * TEXTURES_PER_MATERIAL + j]);
		}
		iMeshes[i]->setMaterial(Mesh::Material(roomShader, currTexs, Mesh::Material::Opaque::YES));
	}
	debug =! debug;
}
