#include "Glimpse.h"

#include <PGE/Graphics/Material.h>

#include "../Graphics/Resources.h"
#include "../Utilities/Directories.h"

using namespace PGE;

Glimpse::Glimpse(Resources& res) : resources(res) {
    Graphics& graphics = resources.getGraphics();
    StructuredData data(resources.getGlimpseShader().getVertexLayout(), 4);
    data.setValue(0, "position", Vector3f(-50, -50, 0)); data.setValue(1, "position", Vector3f(50, -50, 0));
    data.setValue(2, "position", Vector3f(-50, 50, 0)); data.setValue(3, "position", Vector3f(50, 50, 0));
    data.setValue(0, "uv", Vector2f(0.f, 1.f)); data.setValue(1, "uv", Vector2f(1.f, 1.f));
    data.setValue(2, "uv", Vector2f(0.f, 0.f)); data.setValue(3, "uv", Vector2f(1.f, 0.f));
    glimpseMesh = Mesh::create(graphics);
    glimpseMesh->setGeometry(std::move(data), Mesh::PrimitiveType::TRIANGLE, { 0, 1, 2, 3, 2, 1 });
    glimpseTex = resources.getTexture(Directories::TEXTURES + "glimpse.ktx2", Texture::CompressedFormat::BC3);
    material = Material::create(graphics, resources.getGlimpseShader(), *glimpseTex, Material::Opaque::YES);
    glimpseMesh->setMaterial(material);
}

Glimpse::~Glimpse() {
    delete glimpseMesh;
    delete material;
    glimpseTex.drop();
}

void Glimpse::setPosition(const Vector3f& inPos) {
	glimpsePos = inPos;
}

void Glimpse::render() {
    glimpseMesh->render();
}

void Glimpse::update(const Vector3f& direction) {
    resources.getGlimpseShader().getVertexShaderConstant("worldMatrix").setValue(
        Matrix4x4f::translate(glimpsePos) * Matrix4x4f::lookAt(glimpsePos, direction)
    );
}
