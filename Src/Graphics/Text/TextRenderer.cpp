#include "TextRenderer.h"

#include <PGE/Graphics/Graphics.h>

using namespace PGE;

TextRenderer::TextRenderer(Resources& res, const Font& fnt)
	: font(fnt),
	positionConstant(res.getTextShader().getVertexShaderConstant("position")) {
	material = Mesh::Material(res.getTextShader(), fnt.getTexture(), Mesh::Material::Opaque::NO);
	mesh = Mesh::create(res.getGraphics());
	mesh->setMaterial(material);
}

TextRenderer::~TextRenderer() {
	delete mesh;
}

void TextRenderer::setPosition(const Vector2f& pos) {
	position = pos;
}

void TextRenderer::setText(const String& txt) {
	StructuredData data(material.getShader().getVertexLayout(), txt.length() * 4);
	std::vector<Mesh::Triangle> primitives;
	int i = 0;
	float x = scale * font.getGlyph(*txt.begin())->bearing.x;
	float slant = italic ? 0.15f : 0.f;
	for (char16 ch : txt) {
		Font::Glyph* glyph = font.getGlyph(ch);
		data.setValue(i + 0, "position", Vector2f(x - scale * (glyph->bearing.x + slant), scale * glyph->bearing.y));
		data.setValue(i + 0, "uv", glyph->uv.topLeftCorner());
		data.setValue(i + 1, "position", Vector2f(x - scale * (glyph->size.x + glyph->bearing.x + slant), scale * glyph->bearing.y));
		data.setValue(i + 1, "uv", glyph->uv.topRightCorner());
		data.setValue(i + 2, "position", Vector2f(x - scale * glyph->bearing.x, scale * (glyph->bearing.y - glyph->size.y)));
		data.setValue(i + 2, "uv", glyph->uv.bottomLeftCorner());
		data.setValue(i + 3, "position", Vector2f(x - scale * (glyph->size.x + glyph->bearing.x), scale * (glyph->bearing.y - glyph->size.y)));
		data.setValue(i + 3, "uv", glyph->uv.bottomRightCorner());
		primitives.emplace_back(Mesh::Triangle(i + 2, i + 1, i + 0));
		primitives.emplace_back(Mesh::Triangle(i + 1, i + 2, i + 3));
		i += 4;
		x -= scale * glyph->advance;
	}

	mesh->setGeometry(std::move(data), primitives);
}

void TextRenderer::render() const {
	positionConstant.setValue(position);
	mesh->render();
}
