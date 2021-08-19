#ifndef B_TEXTRENDERER_H_INCLUDED
#define B_TEXTRENDERER_H_INCLUDED

#include "Font.h"

class TextRenderer {
	public:
		TextRenderer(Resources& res, const Font& fnt);
		~TextRenderer();

		void setPosition(const PGE::Vector2f& pos);
		void setText(const PGE::String& txt);

		void render() const;

	private:
		const Font& font;

		bool italic = false;
		float scale = 10.f;
		
		PGE::Vector2f position;
		PGE::Shader::Constant& positionConstant;

		PGE::Mesh::Material material;
		PGE::Mesh* mesh;

		
};

#endif // B_TEXTRENDERER_H_INCLUDED
