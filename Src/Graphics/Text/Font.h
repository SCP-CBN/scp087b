#ifndef B_FONT_H_INCLUDED
#define B_FONT_H_INCLUDED

#include <PGE/Math/Rectangle.h>
#include <PGE/Graphics/Mesh.h>

#include "../Resources.h"

class Font {
	public:
		struct Glyph {
			PGE::Rectanglef uv;
			PGE::Vector2f bearing;
			PGE::Vector2f size;
			float advance;
		};

		Font(Resources& res, const PGE::FilePath& path);
		~Font();

		PGE::Texture& getTexture() const;
		Glyph* getGlyph(PGE::char16 ch) const;

	private:
		Resources::Handle<PGE::Texture> textMap;

		std::unordered_map<PGE::char16, std::shared_ptr<Glyph>> glyphs;
};

#endif // B_FONT_H_INCLUDED
