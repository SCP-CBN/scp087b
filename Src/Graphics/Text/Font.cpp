#include "Font.h"

#include <PGE/File/BinaryReader.h>
#include <PGE/Graphics/Graphics.h>

using namespace PGE;

Font::Font(Resources& res, const FilePath& path) {
	textMap = res.getTexture(path + ".png", Texture::Format::R8, false);

	BinaryReader br(path + ".fnt");
	u32 size = br.read<u32>();
	float ar = res.getGraphics().getAspectRatio();
	String chars;
	while (true) {
		chars = br.read<String>();
		if (chars.isEmpty()) {
			break;
		}
		i32 x = br.read<i32>(); i32 y = br.read<i32>();
		i32 width = br.read<i32>(); i32 height = br.read<i32>();
		i32 xBearing = br.read<i32>(); i32 yBearing = br.read<i32>();
		i32 advance = br.read<i32>();
		std::shared_ptr<Glyph> sp = std::make_shared<Glyph>(Glyph{
			Rectanglef::fromDims(
				(x + 0.5f) / textMap->getWidth(),
				(y + 0.5f) / textMap->getHeight(),
				(width - 1.f) / textMap->getWidth(),
				(height - 1.f) / textMap->getHeight()
			),
			Vector2f((float)xBearing / size / ar, (float)yBearing / size),
			Vector2f((float)width / size / ar, (float)height / size),
			(float)advance / size / ar
		});
		for (char16 ch : chars) {
			glyphs.emplace(ch, sp);
		}
	}

	charHeight = glyphs.find('T')->second->size.y;
}

Font::~Font() {
	textMap.drop();
}

Texture& Font::getTexture() const {
	return *textMap;
}

Font::Glyph& Font::getGlyph(char16 ch) const {
	return *glyphs.find(ch)->second.get();
}

float Font::getHeight() const {
	return charHeight;
}
