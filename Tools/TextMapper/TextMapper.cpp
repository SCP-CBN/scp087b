#include <iostream>
#include <execution>

#include <PGE/Init/Init.h>
#include <PGE/File/FilePath.h>
#include <PGE/File/BinaryWriter.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Libraries/lodepng/lodepng.h"

#include "Utilities/ConsoleHelper.hpp"
#include "Utilities/AProgressBar.hpp"
#include "Utilities/AThrobber.hpp"

using namespace PGE;

#define ASSERT_FREETYPE(err) { FT_Error FREETYPE_ERROR = err; PGE_ASSERT(FREETYPE_ERROR == FT_Err_Ok, String::from(FREETYPE_ERROR)); } (void)0

static void textMap(const std::vector<String>& args) {
    std::cout << "Extracting char ranges..." << std::endl;
    struct CharRange {
        char16 from;
        char16 to;
    };
    int totalChars = 0;
    std::vector<CharRange> ranges;
    std::vector<String> strs = args[1].split(";", false);
    ranges.reserve(strs.size());
    for (const String& str : strs) {
        std::vector<String> parts = str.split("-", false);
        CharRange range;
        if (parts.size() == 2) {
            range = { (char16)parts[0].to<u16>(), (char16)parts[1].to<u16>() };
            if (range.from > range.to) { std::swap(range.from, range.to); }
        } else if (parts.size() == 1) {
            char16 ch = parts[0].to<u16>();
            range = { ch, (char16)(ch + 1) };
        } else {
            throw PGE_CREATE_EX("Provide characters either as single character codes (e.g. '33'), or as a range (e.g.: '65-91')");
        }

        ranges.emplace_back(range);
        totalChars += range.to - range.from;
    }

    String file;
    if (*args[0].charAt(0) == '"' && *args[0].charAt(args[0].length() - 1) == '"') {
        file = args[0].substr(1, args[0].length() - 2);
    } else {
        file = args[0];
    }

    u32 fntSz = args[2].to<u32>();

    std::cout << "Loading: " << file << std::endl;
    FilePath path = FilePath::fromStr(file);
    PGE_ASSERT(path.exists(), "Font path does not exist");

    FT_Library library;
    ASSERT_FREETYPE(FT_Init_FreeType(&library));

    FT_Face face;
    ASSERT_FREETYPE(FT_New_Face(library, path.str().cstr(), 0, &face));
    ASSERT_FREETYPE(FT_Set_Pixel_Sizes(face, 0, fntSz));

    std::cout << "Loading glyph sizes..." << std::endl;
    struct Glyph {
        FT_UInt index;
        FT_UInt width;
        FT_UInt height;
    };
    std::vector<Glyph> glyphs; glyphs.reserve(totalChars);
    std::unordered_multimap<FT_UInt, char16> existingIndices;
    for (const CharRange& range : ranges) {
        for (char16 ch = range.from; ch < range.to; ch++) {
            FT_UInt index = FT_Get_Char_Index(face, ch);
            if (index == 0) {
                std::cout << "Warning, Requested character " << ch << " is not represented in the font" << std::endl;
                continue;
            }
            if (auto it = existingIndices.find(index); it == existingIndices.end()) {
                FT_Load_Glyph(face, index, FT_LOAD_NO_BITMAP);
                glyphs.emplace_back(Glyph{ index, face->glyph->bitmap.width, face->glyph->bitmap.rows });
            } else {
                std::cout << "Warning, Requested character " << ch << " is already represented by character " << it->second << std::endl;
            }
            existingIndices.emplace(index, ch);
        }
    }

    PGE_ASSERT(!glyphs.empty(), "No glyphs to draw!");

    std::cout << "Sorting by height..." << std::endl;
    std::sort(std::execution::par_unseq, glyphs.begin(), glyphs.end(), [](const Glyph& a, const Glyph& b) { return a.height < b.height; });

    // TODO: Possibly optimize selected size.
    std::cout << "Determining texture size..." << std::endl;
    FT_UInt width = 1; FT_UInt height = 1;
    bool fits;
    do {
        fits = true;
        if (width == height) {
            width *= 2;
        } else {
            height *= 2;
        }
        FT_UInt x = 0; FT_UInt y = 0;
        for (int i = 0; i < glyphs.size(); i++) {
            x += glyphs[i].width;
            if (x >= width) {
                x = glyphs[i].width;
                if (i == 0) {
                    fits = false;
                    break;
                }
                y += glyphs[i - 1].height;
                if (y >= height) {
                    fits = false;
                    break;
                }
            }
        }
        if (y + glyphs.back().height >= height) {
            fits = false;
        }
    } while (!fits);

    std::cout << "Rendering glyphs:" << std::endl;
    ProgressBar pb((int)glyphs.size(), "<[", "]>", '=', '-', std::min(75, ConsoleHelper::getSize().x));
    BinaryWriter bw(path.trimExtension() + ".fnt");
    bw.write<u32>(fntSz);
    int xOff = 0;
    int yOff = 0;
    std::vector<byte> buffer(width * height);
    String str;
    for (int i = 0; i < glyphs.size(); i++) {
        pb.update(i);
        if (xOff + glyphs[i].width >= width) {
            xOff = 0;
            yOff += glyphs[i - 1].height;
        }

        FT_Load_Glyph(face, glyphs[i].index, FT_LOAD_RENDER);
        for (FT_UInt y = 0; y < glyphs[i].height; y++) {
            for (FT_UInt x = 0; x < glyphs[i].width; x++) {
                buffer[(y + yOff) * width + x + xOff] = face->glyph->bitmap.buffer[y * glyphs[i].width + x];
            }
        }

        str = String();
        const auto& chars = existingIndices.equal_range(glyphs[i].index);
        for (auto it = chars.first; it != chars.second; it++) {
            str += it->second;
        }
        bw.write<String>(str);

        bw.write<i32>(xOff);
        bw.write<i32>(yOff);
        bw.write<i32>(glyphs[i].width);
        bw.write<i32>(glyphs[i].height);
        bw.write<i32>(face->glyph->metrics.horiBearingX >> 6);
        bw.write<i32>(face->glyph->metrics.horiBearingY >> 6);
        bw.write<i32>(face->glyph->metrics.horiAdvance >> 6);

        xOff += glyphs[i].width;
    }
    bw.write<byte>(0);
    pb.finish();

    std::cout << "Encoding image:" << std::endl;
    ProgressBarThrobber throbber(std::cout, 10, "<[", "]>", '=', '-', 75, 25);
    throbber.start();
    unsigned ret = lodepng::encode((path.trimExtension() + ".png").str().cstr(), buffer, width, height, LodePNGColorType::LCT_GREY);
    throbber.stop();
    PGE_ASSERT(ret == 0, String("lodepng Encoding error:\n") + lodepng_error_text(ret));

    std::cout << "All done!" << std::endl;
}

int main(int argc, const char** argv) {
    const char* argNames[] = { "font", "chars", "size" };
    std::vector<String> args(sizeof(argNames) / sizeof(*argNames));
    if (argc < 1 || argc > args.size()) {
        std::cout << "Wrong amount of arguments, expected: 0 - " << args.size() << std::endl;
        std::cout << "Asking for all arguments manually" << std::endl;
        argc = 1;
    }
    for (int i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
    }
    for (int i = argc; i <= args.size(); i++) {
        std::cout << "Enter " << argNames[i - 1] << ": ";
        std::cin >> args[i - 1];
    }
#ifdef _DEBUG
    try {
#endif
        ConsoleHelper::setCursorVisibility(false);
        textMap(args);
        ConsoleHelper::setCursorVisibility(true);
        return 0;
#ifdef _DEBUG
    } catch (const Exception& e) {
        std::cout << "Mapping failed! Reason:" << std::endl << e.what() << std::endl;
        ConsoleHelper::setCursorVisibility(true);
        return 1;
    }
#endif
}
