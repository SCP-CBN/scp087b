#include "Resources.h"

#include <spng.h>
#include <gli/gli.hpp>
#include <ktx.h>

#include "Camera.h"
#include "../Utilities/Directories.h"

using namespace PGE;

Resources::Resources(Graphics& gfx, Camera& cam) : graphics(gfx), camera(cam) {
	textShader = std::unique_ptr<Shader>(Shader::load(gfx, Directories::SHADERS + "Text"));
	roomShader = std::unique_ptr<Shader>(Shader::load(gfx, Directories::SHADERS + "Room"));
	glimpseShader = std::unique_ptr<Shader>(Shader::load(gfx, Directories::SHADERS + "Glimpse"));

	roomShader->getFragmentShaderConstant("effectiveRangeSquared").setValue(300.f * 300.f);
	textShader->getVertexShaderConstant("projectionMatrix").setValue(Matrix4x4f::constructOrthographicMat(100.f, 100.f, 0.1f, 1.f));

	cam.addShader(*roomShader);
	cam.addShader(*glimpseShader);
}

Resources::Handle<Texture> Resources::getTexture(const FilePath& path, const Texture::AnyFormat& fmt, bool mipmaps) {
	// This does not support the same texture being loaded with and without mipmaps.
	// Can be easily added if needed.
	const String& name = path.str();

	Texture* tex = textureCache.tryGet(name);

	if (tex == nullptr) {
		std::vector<byte> bytes = path.readBytes();

		if (std::holds_alternative<Texture::Format>(fmt)) {
			std::unique_ptr<spng_ctx, decltype(&spng_ctx_free)> context(spng_ctx_new(0), &spng_ctx_free);
			spng_set_png_buffer(context.get(), bytes.data(), bytes.size());

			spng_format spngFmt;

			Texture::Format ufmt = std::get<Texture::Format>(fmt);
			switch (ufmt) {
				case Texture::Format::RGBA64: {
					spngFmt = SPNG_FMT_RGBA16;
				} break;
				case Texture::Format::RGBA32: {
					spngFmt = SPNG_FMT_RGBA8;
				} break;
				case Texture::Format::R8: {
					spngFmt = SPNG_FMT_G8;
				} break;
				default: {
					throw PGE_CREATE_EX("Invalid Format " + String::from((int)ufmt));
				}
			}

			size_t size;
			spng_decoded_image_size(context.get(), spngFmt, &size);

			spng_ihdr meta;
			spng_get_ihdr(context.get(), &meta);

			std::vector<byte> raw(size);
			spng_decode_image(context.get(), raw.data(), size, spngFmt, 0);

			tex = Texture::load(graphics, meta.width, meta.height, raw.data(), ufmt, mipmaps);
		} else {
			Texture::CompressedFormat cfmt = std::get<Texture::CompressedFormat>(fmt);

			std::vector<Texture::Mipmap> mipmaps;

			if (path.getExtension() == "dds") {
				gli::texture glTex = gli::load_dds((char*)bytes.data(), bytes.size());
				mipmaps.reserve(glTex.levels());
				for (int i = 0; i < glTex.levels(); i++) {
					mipmaps.emplace_back(glTex.extent(i).x, glTex.extent(i).y, (byte*)glTex.data(0, 0, i), glTex.size(i));
				}
				tex = Texture::loadCompressed(graphics, mipmaps, cfmt);
			} else {
				ktxTexture* ktxTex;
				ktxTexture_CreateFromMemory(bytes.data(), bytes.size(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTex);
				mipmaps.resize(ktxTex->numLevels);
				ktxTexture_IterateLevels(ktxTex, [](int miplevel, int face, int width, int height, int depth, ktx_uint64_t faceLodSize, void* pixels, void* userdata) {
					(*((std::vector<Texture::Mipmap>*)userdata))[miplevel] = Texture::Mipmap(width, height, (byte*)pixels, faceLodSize);
					return (KTX_error_code)0;
				}, &mipmaps);
				tex = Texture::loadCompressed(graphics, mipmaps, cfmt);
				ktxTexture_Destroy(ktxTex);
			}
		}

		textureCache.add(name, tex);
	}

	return Handle<Texture>(tex, path.str(), &textureCache);
}

Shader& Resources::getRoomShader() const {
	return *roomShader;
}

Shader& Resources::getTextShader() const {
	return *textShader;
}

Shader& Resources::getGlimpseShader() const {
	return *glimpseShader;
}

Graphics& Resources::getGraphics() const {
	return graphics;
}
