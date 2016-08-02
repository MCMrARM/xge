#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "MeshBuilderConfig.h"

struct FT_LibraryRec_;
struct FT_FaceRec_;

namespace xge {

    class Texture;
    class Mesh;
    class UTF8String;

    struct FontMesh {

        std::vector<std::shared_ptr<Mesh>> meshes;

        void draw();

    };

    class Font {

    private:
        struct FontChar {
            glm::vec2 uv1, uv2;
            float offX, offY, adv, w, h;
        };
        struct FontAtlas {
            std::map<int, FontChar> chars;
            std::shared_ptr<Texture> tex;
        };

        static FT_LibraryRec_ *ftLibrary;

        static const char *DEFAULT_CHARS;
        static std::vector<unsigned int> defaultChars;

        static std::vector<unsigned int> &getDefaultChars();

        void load(std::vector<char> data, unsigned int charHeight);
        FontAtlas buildAtlas(const std::vector<unsigned int> &characters);

        const unsigned int charsPerUnicodeAtlas;

        FT_FaceRec_ *face = nullptr;
        std::vector<char> fontData;
        FontAtlas defaultAtlas;
        std::map<unsigned int, FontAtlas> unicodeAtlases;

        std::pair<FontAtlas *, FontChar *> getChar(unsigned int ch);

        float getSpacing(unsigned int char1, unsigned int char2);

    public:
        Font(std::vector<char> data, unsigned int charHeight, unsigned int charsPerUnicodeAtlas = 256);

        std::shared_ptr<FontMesh> buildASCII(const MeshBuilderConfig &config, float x, float y, const std::string &text,
                                             glm::vec4 color);
        std::shared_ptr<FontMesh> buildUTF8(const MeshBuilderConfig &config, float x, float y, const UTF8String &text,
                                            glm::vec4 color);

        float getWidthASCII(const std::string &text);
        float getWidthUTF8(const UTF8String &text);

    };

}