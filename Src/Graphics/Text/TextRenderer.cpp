#include "TextRenderer.h"

#include <PGE/Graphics/Graphics.h>
#include <PGE/Graphics/Material.h>

using namespace PGE;

TextRenderer::TextRenderer(Resources& res, const Font& fnt)
	: font(fnt),
	positionConstant(res.getTextShader().getVertexShaderConstant("position")) {
	material = Material::create(res.getGraphics(), res.getTextShader(), fnt.getTexture(), Material::Opaque::NO);
	mesh = Mesh::create(res.getGraphics());
	mesh->setMaterial(material);
	mesh->setUpdateStrategy(Mesh::UpdateStrategy::PER_FRAME);
}

TextRenderer::~TextRenderer() {
	delete material;
	delete mesh;
}

void TextRenderer::setItalic(bool ital) {
	italic = ital;
}

void TextRenderer::setScale(float scl) {
	// TODO: Force reconstruction?
	scale = scl;
}

void TextRenderer::setPosition(const Vector2f& pos) {
	position = pos;
}

void TextRenderer::setText(const String& txt) {
	StructuredData data(material->getShader().getVertexLayout(), txt.length() * 4);
	std::vector<Mesh::Triangle> primitives;

	if (!txt.isEmpty()) {
		int i = 0;
		float x = scale * font.getGlyph(*txt.begin()).bearing.x;
		float y = 0.f;
		float slant = italic ? 0.15f : 0.f;
		for (String::Iterator it = txt.begin(); it != txt.end(); it++) {
			if (*it == '\n') {
				y -= scale * font.getHeight();
				String::Iterator next = it + 1;
				if (next == txt.end()) {
					break;
				}
				x = scale * font.getGlyph(*next).bearing.x;
				continue;
			}
			Font::Glyph& glyph = font.getGlyph(*it);
			data.setValue(i + 0, "position", Vector2f(x - scale * (glyph.bearing.x + slant), y + scale * glyph.bearing.y));
			data.setValue(i + 0, "uv", glyph.uv.topLeftCorner());
			data.setValue(i + 1, "position", Vector2f(x - scale * (glyph.size.x + glyph.bearing.x + slant), y + scale * glyph.bearing.y));
			data.setValue(i + 1, "uv", glyph.uv.topRightCorner());
			data.setValue(i + 2, "position", Vector2f(x - scale * glyph.bearing.x, y + scale * (glyph.bearing.y - glyph.size.y)));
			data.setValue(i + 2, "uv", glyph.uv.bottomLeftCorner());
			data.setValue(i + 3, "position", Vector2f(x - scale * (glyph.size.x + glyph.bearing.x), y + scale * (glyph.bearing.y - glyph.size.y)));
			data.setValue(i + 3, "uv", glyph.uv.bottomRightCorner());
			primitives.emplace_back(i + 2, i + 1, i + 0);
			primitives.emplace_back(i + 1, i + 2, i + 3);
			i += 4;
			x -= scale * glyph.advance;
		}
		width = -x;
	}

	mesh->setGeometry(std::move(data), primitives);
}

float TextRenderer::getWidth() const {
	return width;
}

float TextRenderer::getHeight() const {
	return font.getHeight() * scale;
}

void TextRenderer::render() const {
	positionConstant.setValue(position);
	mesh->render();
}
