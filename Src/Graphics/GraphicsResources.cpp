#include "GraphicsResources.h"

#include <spng.h>

using namespace PGE;

GraphicsResources::GraphicsResources(Graphics& gfx) : graphics(gfx) { }

GraphicsResources::Handle<Texture> GraphicsResources::getTexture(const FilePath& path, bool mipmaps) {
	const String& name = path.str();

	Texture* tex = textureCache.tryGet(name);

	if (tex == nullptr) {
		std::vector<byte> bytes = path.readBytes();

		std::unique_ptr<spng_ctx, decltype(&spng_ctx_free)> context(spng_ctx_new(0), &spng_ctx_free);
		spng_set_png_buffer(context.get(), bytes.data(), bytes.size());

		size_t size;
		spng_decoded_image_size(context.get(), SPNG_FMT_RGBA8, &size);

		spng_ihdr meta;
		spng_get_ihdr(context.get(), &meta);

		std::vector<byte> raw(size);
		spng_decode_image(context.get(), raw.data(), size, SPNG_FMT_RGBA8, 0);

		tex = Texture::load(graphics, meta.width, meta.height, raw.data(), Texture::Format::RGBA32, mipmaps);

		textureCache.add(name, tex);
	}

	return Handle<Texture>(tex, path.str(), &textureCache);
}

GraphicsResources::Handle<Shader> GraphicsResources::getShader(const FilePath& path) {
	const String& name = path.str();

	Shader* sh = shaderCache.tryGet(name);

	if (sh == nullptr) {
		sh = Shader::load(graphics, path);

		shaderCache.add(name, sh);
	}

	return Handle<Shader>(sh, name, &shaderCache);
}
