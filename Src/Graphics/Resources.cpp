#include "Resources.h"

#include <spng.h>

#include "Camera.h"
#include "../Utilities/Directories.h"

using namespace PGE;

Resources::Resources(Graphics& gfx, Camera& cam) : graphics(gfx), camera(cam) {
	roomShader = std::unique_ptr<Shader>(Shader::load(gfx, Directories::SHADERS + "Room"));
	cam.addShader(*roomShader);
}

Resources::Handle<Texture> Resources::getTexture(const FilePath& path, bool mipmaps) {
	// This does not support the same texture being loaded with and without mipmaps.
	// Can be easily added if needed.
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

Shader& Resources::getRoomShader() const {
	return *roomShader;
}

Graphics& Resources::getGraphics() const {
	return graphics;
}
