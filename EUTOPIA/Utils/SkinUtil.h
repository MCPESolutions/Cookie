#pragma once
#include <string>
#include <vector>

#include "../SDK/Core/mce.h"
#pragma warning(disable : 4996)
#ifndef SVPNG_PUT
#define SVPNG_PUT(u) fputc(u, fp)
#endif

class SkinUtils {
   public:
    // 保存完整皮肤到文件的方法
    static inline void saveFullSkin(const mce::Image& skinImage, const std::string& filename) {
        if(!skinImage.mImageBytes.mBlob || skinImage.mWidth <= 0 || skinImage.mHeight <= 0) {
            return;
        }

        FILE* fp = fopen(filename.c_str(), "wb");
        if(!fp) {
            return;
        }

        svpng(fp, skinImage.mWidth, skinImage.mHeight, skinImage.mImageBytes.mBlob.get(), 1);
        fclose(fp);
    }

    // 获取完整皮肤数据的方法 - 用于渲染
    static inline std::vector<unsigned char> getFullSkinData(const mce::Image& skinImage) {
        if(!skinImage.mImageBytes.mBlob || skinImage.mWidth <= 0 || skinImage.mHeight <= 0) {
            return std::vector<unsigned char>();
        }

        size_t dataSize = skinImage.mWidth * skinImage.mHeight * 4;  // RGBA
        std::vector<unsigned char> skinData(dataSize);

        std::memcpy(skinData.data(), skinImage.mImageBytes.mBlob.get(), dataSize);

        return skinData;
    }

    // 改进的头部截取方法
    static inline std::vector<unsigned char> cropHeadImproved(const mce::Image& originalImage,
                                                              bool includeHat = true) {
        if(!originalImage.mImageBytes.mBlob || originalImage.mWidth <= 0 ||
           originalImage.mHeight <= 0) {
            return std::vector<unsigned char>();
        }

        int width = originalImage.mWidth;
        int height = originalImage.mHeight;

        // 支持常见的皮肤格式
        int headX, headY, headWidth, headHeight;
        int hatX, hatY, hatWidth, hatHeight;

        if(width == 64 && height == 64) {
            // 64x64 新格式皮肤
            headX = 8;
            headY = 8;
            headWidth = 8;
            headHeight = 8;
            hatX = 40;
            hatY = 8;
            hatWidth = 8;
            hatHeight = 8;
        } else if(width == 64 && height == 32) {
            // 64x32 经典格式皮肤
            headX = 8;
            headY = 8;
            headWidth = 8;
            headHeight = 8;
            hatX = 40;
            hatY = 8;
            hatWidth = 8;
            hatHeight = 8;
        } else {
            // 尝试按比例缩放坐标
            float scale = width / 64.0f;
            headX = (int)(8 * scale);
            headY = (int)(8 * scale);
            headWidth = (int)(8 * scale);
            headHeight = (int)(8 * scale);
            hatX = (int)(40 * scale);
            hatY = (int)(8 * scale);
            hatWidth = (int)(8 * scale);
            hatHeight = (int)(8 * scale);
        }

        const int bytesPerPixel = 4;  // RGBA
        std::vector<unsigned char> headImageBytes(headWidth * headHeight * bytesPerPixel);

        // 首先复制基础头部
        for(int y = 0; y < headHeight; ++y) {
            for(int x = 0; x < headWidth; ++x) {
                int originalX = headX + x;
                int originalY = headY + y;

                if(originalX >= originalImage.mWidth || originalY >= originalImage.mHeight) {
                    continue;
                }

                int originalIndex = (originalY * originalImage.mWidth + originalX) * bytesPerPixel;
                int headIndex = (y * headWidth + x) * bytesPerPixel;

                // 复制 RGBA
                for(int i = 0; i < bytesPerPixel; ++i) {
                    headImageBytes[headIndex + i] =
                        originalImage.mImageBytes.mBlob[originalIndex + i];
                }
            }
        }

        // 如果需要包含帽子层，则叠加帽子
        if(includeHat && (hatX + hatWidth <= originalImage.mWidth) &&
           (hatY + hatHeight <= originalImage.mHeight)) {
            for(int y = 0; y < hatHeight; ++y) {
                for(int x = 0; x < hatWidth; ++x) {
                    int hatPixelX = hatX + x;
                    int hatPixelY = hatY + y;
                    int hatIndex = (hatPixelY * originalImage.mWidth + hatPixelX) * bytesPerPixel;

                    // 检查帽子像素的透明度
                    unsigned char alpha = originalImage.mImageBytes.mBlob[hatIndex + 3];
                    if(alpha > 0) {  // 如果不完全透明，则覆盖基础头部
                        int headIndex = (y * headWidth + x) * bytesPerPixel;
                        for(int i = 0; i < bytesPerPixel; ++i) {
                            headImageBytes[headIndex + i] =
                                originalImage.mImageBytes.mBlob[hatIndex + i];
                        }
                    }
                }
            }
        }

        return headImageBytes;
    }

    static inline void svpng(FILE* fp, unsigned w, unsigned h, const unsigned char* img,
                             int alpha) {
        static const unsigned t[] = {0,
                                     0x1db71064,
                                     0x3b6e20c8,
                                     0x26d930ac,
                                     0x76dc4190,
                                     0x6b6b51f4,
                                     0x4db26158,
                                     0x5005713c,
                                     /* CRC32 Table */ 0xedb88320,
                                     0xf00f9344,
                                     0xd6d6a3e8,
                                     0xcb61b38c,
                                     0x9b64c2b0,
                                     0x86d3d2d4,
                                     0xa00ae278,
                                     0xbdbdf21c};
        unsigned a = 1, b = 0, c, p = w * (alpha ? 4 : 3) + 1, x, y,
                 i; /* ADLER-a, ADLER-b, CRC, pitch */
#define SVPNG_U8A(ua, l)   \
    for(i = 0; i < l; i++) \
        SVPNG_PUT((ua)[i]);
#define SVPNG_U32(u)                  \
    do {                              \
        SVPNG_PUT((u) >> 24);         \
        SVPNG_PUT(((u) >> 16) & 255); \
        SVPNG_PUT(((u) >> 8) & 255);  \
        SVPNG_PUT((u) & 255);         \
    } while(0)
#define SVPNG_U8C(u)              \
    do {                          \
        SVPNG_PUT(u);             \
        c ^= (u);                 \
        c = (c >> 4) ^ t[c & 15]; \
        c = (c >> 4) ^ t[c & 15]; \
    } while(0)
#define SVPNG_U8AC(ua, l)  \
    for(i = 0; i < l; i++) \
    SVPNG_U8C((ua)[i])
#define SVPNG_U16LC(u)               \
    do {                             \
        SVPNG_U8C((u) & 255);        \
        SVPNG_U8C(((u) >> 8) & 255); \
    } while(0)
#define SVPNG_U32C(u)                 \
    do {                              \
        SVPNG_U8C((u) >> 24);         \
        SVPNG_U8C(((u) >> 16) & 255); \
        SVPNG_U8C(((u) >> 8) & 255);  \
        SVPNG_U8C((u) & 255);         \
    } while(0)
#define SVPNG_U8ADLER(u)       \
    do {                       \
        SVPNG_U8C(u);          \
        a = (a + (u)) % 65521; \
        b = (b + a) % 65521;   \
    } while(0)
#define SVPNG_BEGIN(s, l) \
    do {                  \
        SVPNG_U32(l);     \
        c = ~0U;          \
        SVPNG_U8AC(s, 4); \
    } while(0)
#define SVPNG_END() SVPNG_U32(~c)
        SVPNG_U8A("\x89PNG\r\n\32\n", 8); /* Magic */
        SVPNG_BEGIN("IHDR", 13);          /* IHDR chunk { */
        SVPNG_U32C(w);
        SVPNG_U32C(h); /*   Width & Height (8 bytes) */
        SVPNG_U8C(8);
        SVPNG_U8C(alpha ? 6 : 2); /*   Depth=8, Color=True color with/without alpha (2 bytes) */
        SVPNG_U8AC("\0\0\0", 3);  /*   Compression=Deflate, Filter=No, Interlace=No (3 bytes) */
        SVPNG_END();              /* } */
        SVPNG_BEGIN("IDAT", 2 + h * (5 + p) + 4); /* IDAT chunk { */
        SVPNG_U8AC("\x78\1", 2);                  /*   Deflate block begin (2 bytes) */
        for(y = 0; y < h; y++) {
            /*   Each horizontal line makes a block for simplicity */
            SVPNG_U8C(y == h - 1); /*   1 for the last block, 0 for others (1 byte) */
            SVPNG_U16LC(p);
            SVPNG_U16LC(~p); /*   Size of block in little endian and its 1's complement (4 bytes) */
            SVPNG_U8ADLER(0); /*   No filter prefix (1 byte) */
            for(x = 0; x < p - 1; x++, img++)
                SVPNG_U8ADLER(*img); /*   Image pixel data */
        }
        SVPNG_U32C((b << 16) | a); /*   Deflate block end with adler (4 bytes) */
        SVPNG_END();               /* } */
        SVPNG_BEGIN("IEND", 0);
        SVPNG_END(); /* IEND chunk {} */
    };

    static inline void saveImage(const char* filename, const unsigned char* img, int width,
                                 int height) {
        FILE* fp = fopen(filename, "wb");
        if(!fp) {
            return;
        }
        svpng(fp, width, height, img, 1);  // 1 means with alpha channel
        fclose(fp);
    }

    static inline std::vector<unsigned char> cropHead(const mce::Image& originalImage, int type,
                                                      bool head2 = false) {
        const int headX = head2 ? (type == 0 ? 40 : 80) : (type == 0 ? 8 : 16);
        const int headY = type == 0 ? 8 : 16;
        const int headWidth = type == 0 ? 8 : 16;
        const int headHeight = type == 0 ? 8 : 16;
        const int bytesPerPixel = 4;  // RGBA

        if(originalImage.mWidth < (headX + headWidth) ||
           originalImage.mHeight < (headY + headHeight)) {
            return std::vector<unsigned char>();
        }

        std::vector<unsigned char> headImageBytes(headWidth * headHeight * bytesPerPixel);

        for(int y = 0; y < headHeight; ++y) {
            for(int x = 0; x < headWidth; ++x) {
                int originalX = headX + x;
                int originalY = headY + y;
                int originalIndex = (originalY * originalImage.mWidth + originalX) * bytesPerPixel;
                int headIndex = (y * headWidth + x) * bytesPerPixel;

                for(int i = 0; i < bytesPerPixel; ++i) {
                    headImageBytes[headIndex + i] =
                        originalImage.mImageBytes.mBlob[originalIndex + i];
                }
            }
        }

        return headImageBytes;
    }

    static inline std::vector<unsigned char> resizeHead(const std::vector<unsigned char>& input,
                                                        int width, int height, float scale) {
        unsigned newWidth = static_cast<unsigned>(width * scale);
        unsigned newHeight = static_cast<unsigned>(height * scale);

        std::vector<unsigned char> output(newWidth * newHeight * 4);

        for(unsigned newY = 0; newY < newHeight; ++newY) {
            for(unsigned newX = 0; newX < newWidth; ++newX) {
                float srcX = newX / scale;
                float srcY = newY / scale;

                unsigned srcXI = static_cast<unsigned>(srcX);
                unsigned srcYI = static_cast<unsigned>(srcY);

                if(srcXI >= static_cast<unsigned>(width))
                    srcXI = width - 1;
                if(srcYI >= static_cast<unsigned>(height))
                    srcYI = height - 1;

                unsigned srcIdx = 4 * (srcYI * width + srcXI);
                unsigned dstIdx = 4 * (newY * newWidth + newX);
                if(srcIdx + 3 >= input.size()) {
                    continue;
                }
                for(int c = 0; c < 4; ++c) {
                    output[dstIdx + c] = input[srcIdx + c];
                }
            }
        }

        return output;
    }

    static inline float getHeadScale(int baseSize, int skinWidth) {
        return baseSize / (skinWidth / 8);
    }
};