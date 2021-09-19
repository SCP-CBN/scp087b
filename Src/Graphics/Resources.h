#ifndef B_RESOURCES_H_INCLUDED
#define B_RESOURCES_H_INCLUDED

#include <PGE/Graphics/Texture.h>
#include <PGE/Graphics/Shader.h>

#include "Cache.h"

class Resources {
	public:
		template <typename T>
		class Handle {
			public:
				Handle(T* e, const PGE::String::Key& k, Cache<T>* c)
					: element(e), key(k), cache(c) { }
				Handle() = default;
				operator T*() const { return element; }
				T* operator->() { return element; }
				void drop() { cache->drop(key); }

			private:
				T* element;
				PGE::String::Key key;
				Cache<T>* cache;
		};

		Resources(PGE::Graphics& gfx, class Camera& cam);

		Handle<PGE::Texture> getTexture(const PGE::FilePath& path, const PGE::Texture::AnyFormat& fmt = PGE::Texture::Format::RGBA32, bool mipmaps = true);

		PGE::Shader& getRoomShader() const;
		PGE::Shader& getTextShader() const;
		PGE::Shader& getGlimpseShader() const;

		PGE::Graphics& getGraphics() const;

	private:
		Cache<PGE::Texture> textureCache;

		std::unique_ptr<PGE::Shader> roomShader;
		std::unique_ptr<PGE::Shader> textShader;
		std::unique_ptr<PGE::Shader> glimpseShader;

		class Camera& camera;

		PGE::Graphics& graphics;
};

#endif // B_RESOURCES_H_INCLUDED
