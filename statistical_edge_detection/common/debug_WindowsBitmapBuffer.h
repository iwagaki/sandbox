#ifndef DEBUG_WINDOWS_BITMAP_BUFFER_H_
#define DEBUG_WINDOWS_BITMAP_BUFFER_H_

#include <stdint.h>
#include <algorithm>

#include "common/verify.h"
#include "common/debug_IFrameBuffer.h"


#ifdef _WIN32
#define ATTRIBUTE_PACKED
#pragma pack(1)
#else
#define ATTRIBUTE_PACKED __attribute__((packed))
#endif

struct BITMAPFILEHEADER {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} ATTRIBUTE_PACKED;

struct BITMAPINFOHEADER{
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPixPerMeter;
    int32_t  biYPixPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImporant;
} ATTRIBUTE_PACKED;

#ifdef _WIN32
#pragma pack()
#endif
#undef ATTRIBUTE_PACKED


class debug_WindowsBitmapBuffer : public debug_IFrameBuffer
{
public:
    debug_WindowsBitmapBuffer(uint8_t* ptr)
    {
        BITMAPFILEHEADER* pBitmapFileHeader = (BITMAPFILEHEADER*)ptr;
        BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)(ptr + sizeof(BITMAPFILEHEADER));

        m_width = pBitmapInfoHeader->biWidth;
        m_height = pBitmapInfoHeader->biHeight;
        m_bpp = pBitmapInfoHeader->biBitCount >> 3;
        m_ptr = ptr + pBitmapFileHeader->bfOffBits;
        m_slide = (m_width * m_bpp + 3) & ~3;

        VERIFY(pBitmapInfoHeader->biCompression == 0);
    };

    int width() const { return  m_width; }
    int height() const { return m_height; }
    int bpp() const { return m_bpp; }

    debug_FrameBufferPixel pos(int x, int y)
    {
        x = std::min(std::max(x, 0), m_width - 1);
        y = std::min(std::max(y, 0), m_height - 1);

        uint8_t* ptr = m_ptr + (m_height - 1 - y) * m_slide + x * m_bpp;

        return debug_FrameBufferPixel(ptr + 0, ptr + 1, ptr + 2, 0, 0);
    }

private:
    int m_width;
    int m_height;
    int m_bpp;
    int m_slide;

    uint8_t* m_ptr;
};

#endif // DEBUG_WINDOWS_BITMAP_BUFFER_H_
