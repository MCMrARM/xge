#include "Font.h"

#include <xge/util/Log.h>
#include <xge/util/UTF8String.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../util/ImagePacker.h"
#include "MeshBuilder.h"
#include "Texture.h"

using namespace xge;

FT_LibraryRec_ *Font::ftLibrary;
const char *Font::DEFAULT_CHARS = "0123456789qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM!@#$%^&*()-=_+[]\\{}|;':\",./<>?`~ ";
std::vector<unsigned int> Font::defaultChars;

std::vector<unsigned int>& Font::getDefaultChars() {
    if (defaultChars.size() <= 0) {
        size_t len = strlen(Font::DEFAULT_CHARS);
        for (size_t i = 0; i < len; i++) {
            defaultChars.push_back((unsigned int) DEFAULT_CHARS[i]);
        }
    }
    return defaultChars;
}

void FontMesh::draw() {
    for (auto &mesh : meshes)
        mesh->draw();
}

Font::Font(std::vector<char> data, unsigned int charHeight, unsigned int charsPerUnicodeAtlas) :
        charsPerUnicodeAtlas(charsPerUnicodeAtlas) {
    load(std::move(data), charHeight);
    defaultAtlas = buildAtlas(getDefaultChars());
}

void Font::load(std::vector<char> data, unsigned int charHeight) {
    if (ftLibrary == nullptr) {
        FT_Error error = FT_Init_FreeType(&ftLibrary);
        if (error) {
            Log::error("Font", "Failed to initialize Freetype library (%i)", error);
            abort();
        }
    }

    fontData = std::move(data);
    FT_Error error = FT_New_Memory_Face(ftLibrary, (const FT_Byte*) &fontData[0], fontData.size(), 0, &face);
    if (error) {
        Log::error("Font", "Failed to create font face (%i)", error);
        return;
    }
    error = FT_Set_Pixel_Sizes(face, 0, charHeight);
    if (error) {
        Log::error("Font", "Failed to set pixel sizes [%i]", error);
        return;
    }
}

Font::FontAtlas Font::buildAtlas(const std::vector<unsigned int> &characters) {
    XGEAssert(face != nullptr);

    FontAtlas ret;

    ImagePacker imgPacker;
    for (unsigned int ch : characters) {
        FT_Error error = FT_Load_Char(face, ch, FT_LOAD_RENDER);
        if (error) {
            Log::error("Font", "Failed to load char U+%i", ch);
            continue;
        }
        FT_Glyph_Metrics &metrics = face->glyph->metrics;
        size_t bmpSize = face->glyph->bitmap.width * face->glyph->bitmap.rows;
        std::vector<unsigned char> bmp (bmpSize);
        memcpy(&bmp[0], face->glyph->bitmap.buffer, bmpSize);

        std::string charName;
        charName.resize(sizeof(unsigned int));
        memcpy(&charName[0], &ch, sizeof(unsigned int));
        if (face->glyph->bitmap.width != 0)
            imgPacker.add(charName, Image (std::move(bmp), face->glyph->bitmap.width, face->glyph->bitmap.rows,
                                           ImageFormat::GRAY));

        ret.chars[ch] = { glm::vec2(), glm::vec2(), (float) face->glyph->bitmap_left,
                          (metrics.horiBearingY - metrics.height) / 64.f, metrics.horiAdvance / 64.f,
                          metrics.width / 64.f, metrics.height / 64.f };
    }
    auto packerRet = imgPacker.build();
    if (!packerRet) {
        Log::error("Font", "Failed to build atlas");
        abort();
    }
    std::shared_ptr<Texture> tex(new Texture(packerRet.image));
    ret.tex = tex;
    for (const auto &p : packerRet.packedItems) {
        unsigned int chId = *((const unsigned int *) p.first.data());
        FontChar &ch = ret.chars.at(chId);
        auto uv = packerRet.getItemUVSet(p.second);
        ch.uv1 = uv.first;
        ch.uv2 = uv.second;
    }

    return std::move(ret);
}

std::pair<Font::FontAtlas *, Font::FontChar *> Font::getChar(unsigned int ch) {
    FontAtlas *atlas = nullptr;
    if (ch < 128) {
        atlas = &defaultAtlas;
    } else {
        unsigned int atlasId = (ch - 128) / charsPerUnicodeAtlas;
        if (unicodeAtlases.count(atlasId) > 0) {
            atlas = &unicodeAtlases.at(atlasId);
        } else {
            std::vector<unsigned int> chars;
            unsigned int baseId = atlasId * charsPerUnicodeAtlas + 128;
            for (int i = 0; i < charsPerUnicodeAtlas; i++)
                chars.push_back(baseId + i);
            unicodeAtlases.insert({ atlasId, buildAtlas(chars) });
            atlas = &unicodeAtlases.at(atlasId);
        }
    }
    if (atlas == nullptr || atlas->chars.count(ch) < 0)
        return { nullptr, nullptr };
    return { atlas, &atlas->chars.at(ch) };
}

std::shared_ptr<FontMesh> Font::buildASCII(const MeshBuilderConfig &config, float x, float y, const std::string &text,
                                           glm::vec4 color) {
    char pc = 0;
    MeshBuilder builder (config);
    builder.setPrimaryTexture(defaultAtlas.tex);
    for (char c : text) {
        if (defaultAtlas.chars.count((unsigned int) c) <= 0)
            continue;
        const auto &cd = defaultAtlas.chars.at((unsigned int) c);

        if (pc != 0)
            x += getSpacing((unsigned int) pc, (unsigned int) c);

        builder.rect({x + cd.offX, y + cd.offY}, {x + cd.offX + cd.w, y + cd.offY + cd.h}, cd.uv1, cd.uv2, color);
        x += cd.adv;
        pc = c;
    }
    std::shared_ptr<FontMesh> ret (new FontMesh());
    ret->meshes.push_back(builder.build());
    return std::move(ret);
}

std::shared_ptr<FontMesh> Font::buildUTF8(const MeshBuilderConfig &config, float x, float y, const UTF8String &text,
                                          glm::vec4 color) {
    unsigned int pc = 0;
    std::map<FontAtlas *, MeshBuilder> builders;
    for (UTF8String::Char c : text) {
        auto p = getChar(c);
        if (p.first == nullptr || p.second == nullptr)
            continue;

        const auto &cd = *p.second;
        if (pc != 0)
            x += getSpacing(pc, c);

        if (builders.count(p.first) <= 0) {
            MeshBuilder builder(config);
            builder.setPrimaryTexture(p.first->tex);
            builders.insert({p.first, builder});
        }
        builders.at(p.first).rect({x + cd.offX, y + cd.offY}, {x + cd.offX + cd.w, y + cd.offY + cd.h}, cd.uv1, cd.uv2, color);
        x += cd.adv;
        pc = c;
    }
    //builder.rect({0.f, 0.f}, {400.f, 400.f}, {0.f, 1.f}, {1.f, 0.f}, {1.f, 1.f, 1.f, 1.f});
    std::shared_ptr<FontMesh> ret (new FontMesh());
    for (auto &p : builders)
        ret->meshes.push_back(p.second.build());
    return std::move(ret);
}

float Font::getSpacing(unsigned int char1, unsigned int char2) {
    if (!FT_HAS_KERNING(face))
        return 0;
    FT_UInt index1 = FT_Get_Char_Index(face, char1);
    FT_UInt index2 = FT_Get_Char_Index(face, char2);
    FT_Vector delta;
    FT_Get_Kerning(face, index1, index2, FT_KERNING_DEFAULT, &delta);
    return delta.x / 64.f;
}