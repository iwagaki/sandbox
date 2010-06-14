#ifndef DEBUG_GRAYSCALE_FRAME_BUFFER_H_
#define DEBUG_GRAYSCALE_FRAME_BUFFER_H_

#include <stdint.h>
#include <algorithm>

#include "common/verify.h"
#include "common/debug_IFrameBuffer.h"

class debug_GrayscaleFrameBuffer : public debug_IFrameBuffer
{
public:
    debug_GrayscaleFrameBuffer(int w, int h, int bits, uint8_t* ptr) : m_width(w), m_height(h), m_bpp(bits), m_ptr(ptr) {};
  
    int width() const { return  m_width; }
    int height() const { return m_height; }
    int bpp() const { return m_bpp; }

    debug_FrameBufferPixel pos(int x, int y)
    {
        x = std::min(std::max(x, 0), m_width - 1);
        y = std::min(std::max(y, 0), m_height - 1);

        uint8_t* ptr = m_ptr + (m_width * y + x) * m_bpp;

        return debug_FrameBufferPixel(0, 0, 0, ptr + 0, 0);
    }

private:
    int m_width;
    int m_height;
    int m_bpp;

    uint8_t* m_ptr;
};

#endif // DEBUG_GRAYSCALE_FRAME_BUFFER_H_
