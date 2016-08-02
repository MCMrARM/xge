#include "ImagePacker.h"

#include <cstring>
#include <algorithm>
#include <tuple>
#include <xge/util/DynamicStackArray.h>

using namespace xge;

void ImagePacker::add(std::string name, Image image) {
    ImagePackItem item;
    item.image = std::move(image);
    item.name = std::move(name);
    item.area = item.image.getWidth() * item.image.getHeight();
    items.push_back(std::move(item));
    areItemsSorted = false;
}

ImagePackerResult ImagePacker::build() {
    for (int i = 32; i < 16384; i *= 2) {
        ImagePackerResult result = build(i, i);
        if (!result)
            continue;
        int p2 = 32;
        while (p2 < result.actuallyUsedHeight)
            p2 *= 2;
        if (p2 < result.actuallyUsedHeight) {
            // crop image
            std::vector<unsigned char> newData(result.image.getWidth() * p2 * GetBytesPerPixel(result.image.getFormat()));
            memcpy(newData.data(), result.image.getData().data(), newData.size());
            result.image = Image (newData, result.image.getWidth(), p2, result.image.getFormat());
            result.imgH = p2;
        }
        return std::move(result);
    }
	return ImagePackerResult();
}

ImagePackerResult ImagePacker::build(int width, int height) {
    XGEAssert(items.size() > 0);
    if (!areItemsSorted)
        std::sort(items.begin(), items.end());
	typedef std::pair<int, int> int_pair;
    StackArray(std::vector<int_pair>, v, height);
    for (int i = 0; i < height; i++)
        v[i].push_back({0, width});
    std::vector<std::tuple<int, int, int>> possibilitiesToCheck; // min x, max y, min y
    ImageFormat format = items[0].image.getFormat();
    ImagePackerResult result;
    result.image = Image (std::vector<unsigned char> (width * height * GetBytesPerPixel(format), 0), width, height, format);
    result.imgW = width;
    result.imgH = height;
    for (ImagePackItem &item : items) {
        bool solved = false;
        int itemX = -1;
        int itemY = -1;
        int itemW = item.image.getWidth();
        int itemH = item.image.getHeight();
        for (int i = 0; i < height; i++) {
            auto oldPossibilitiesToCheck = std::move(possibilitiesToCheck);
            for (int_pair &p : v[i]) {
                if (p.second - p.first < itemW)
                    continue;
                if (itemH == 1) {
                    itemX = p.first;
                    itemY = i;
                    solved = true;
                    break;
                }
                bool foundExact = false;
                for (auto pb : oldPossibilitiesToCheck) {
                    if (p.first > std::get<1>(pb))
                        break;
                    int minX = std::max<int>(p.first, std::get<0>(pb));
                    int maxX = std::min(p.second, std::get<1>(pb));
                    if (minX == p.first && maxX == p.second) {
                        foundExact = true;
                    }
                    if (maxX - minX >= itemW) {
                        if (itemH <= i - std::get<2>(pb) + 1) {
                            itemX = minX;
                            itemY = std::get<2>(pb);
                            solved = true;
                            break;
                        }
                        possibilitiesToCheck.push_back(std::make_tuple(minX, maxX, std::get<2>(pb)));
                    }
                }
                if (!foundExact)
                    possibilitiesToCheck.push_back(std::make_tuple(p.first, p.second, i));

                if (solved)
                    break;
            }
            if (solved)
                break;
        }
        //printf("Placing item at %i %i (size: %i %i)\n", itemX, itemY, itemW, itemH);
        if (solved) {
            for (int i = itemY; i < itemY + itemH; i++) {
                for (auto it = v[i].begin(); it != v[i].end(); it++) {
                    if (itemX >= it->first && itemX + itemW <= it->second) {
                        if (itemX == it->first) {
                            it->first = itemX + itemW;
                            if (it->first == it->second) {
                                v[i].erase(it);
                            }
                        } else {
                            int s = it->second;
                            it->second = itemX;
                            if (it->first == it->second) {
                                if (s > itemX + itemW) {
                                    it->first = itemX + itemW;
                                    it->second = s;
                                } else {
                                    v[i].erase(it);
                                }
                            } else if (s > itemX + itemW) {
                                v[i].insert(++it, {itemX + itemW, s});
                            }
                        }
                        break;
                    }
                }
            }
            item.image.copyTo(result.image, itemX, itemY);
            result.packedItems[item.name] = { itemX, itemY, itemW, itemH };
            result.actuallyUsedHeight = std::max(result.actuallyUsedHeight, itemY + itemH);
        } else {
            return ImagePackerResult();
        }
    }
    result.success = true;
    return std::move(result);
}