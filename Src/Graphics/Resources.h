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

		Resources(PGE::Graphics& gfx);

		Handle<PGE::Texture> getTexture(const PGE::FilePath& path, bool mipmaps = true);
		Handle<PGE::Shader> getShader(const PGE::FilePath& path);

		PGE::Graphics& getGraphics() const;

	private:
		Cache<PGE::Texture> textureCache;
		Cache<PGE::Shader> shaderCache;

		PGE::Graphics& graphics;
};

#endif // B_RESOURCES_H_INCLUDED
