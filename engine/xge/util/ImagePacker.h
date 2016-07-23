#pragma once

#include <vector>
#include <map>
#include <glm/vec2.hpp>
#include "Image.h"

namespace xge {

    class ImagePackerResult {

    public:
        struct PackedItem {
            int x, y;
            int w, h;
        };

        bool success = false;
        Image image;
        int imgW, imgH;
        int actuallyUsedHeight = 0;
        std::map<std::string, PackedItem> packedItems;

        operator bool() {
            return success;
        }

        inline void freeImage() {
            image = Image();
        }

        inline PackedItem getItem(const std::string &name) {
            if (packedItems.count(name) > 0)
                return packedItems.at(name);
            return PackedItem();
        }
        inline std::pair<glm::vec2, glm::vec2> getItemUVSet(const PackedItem &item) {
            float imgW = (float) this->imgW;
            float imgH = (float) this->imgH;
            return { { item.x / imgW, item.y / imgH },
                     { (item.x + item.w) / imgW, (item.y + item.h) / imgH } };
        }
        inline std::pair<glm::vec2, glm::vec2> getItemUVSet(const std::string &name) {
            return getItemUVSet(getItem(name));
        }

    };

    class ImagePacker {

    private:
        struct ImagePackItem {
            Image image;
            std::string name;
            int area;

            bool operator <(ImagePackItem const &c) const {
                return area > c.area;
            }
        };
        std::vector<ImagePackItem> items;
        bool areItemsSorted = true;

    public:
        void add(std::string name, Image image);

        /**
         * This function will build a texture with a size being a power of two.
         */
        ImagePackerResult build();

        /**
         * This function will build a texture with the specified size.
         */
        ImagePackerResult build(int width, int height);

    };

}