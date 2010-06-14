#ifndef DEBUG_IFRAME_BUFFER_H_
#define DEBUG_IFRAME_BUFFER_H_

#include <stdint.h>
#include <algorithm>

#include "common/verify.h"

class debug_FrameBufferPixel
{
public:
    debug_FrameBufferPixel(uint8_t* pr, uint8_t* pg, uint8_t* pb, uint8_t* pv = 0, uint8_t* pa = 0) : m_pr(pr), m_pg(pg), m_pb(pb), m_pv(pv), m_pa(pa) {}

    uint8_t& r() const { return *m_pr; }
    uint8_t& g() const { return *m_pg; }
    uint8_t& b() const { return *m_pb; }
    uint8_t& v() const { return *m_pv; }
    uint8_t& a() const { return *m_pa; }

private:
    uint8_t* m_pr;
    uint8_t* m_pg;
    uint8_t* m_pb;
    uint8_t* m_pv;
    uint8_t* m_pa;
};


class debug_IFrameBuffer
{
public:
//    virtual ~debug_IFrameBuffer() = 0;

    virtual int width() const = 0;
    virtual int height() const = 0;
    virtual int bpp() const = 0;
    virtual debug_FrameBufferPixel pos(int x, int y) = 0;
};

#endif // DEBUG_IFRAME_BUFFER_H_
