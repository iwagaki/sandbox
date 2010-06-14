#ifndef DEBUG_X_FRAME_BUFFER_H_
#define DEBUG_X_FRAME_BUFFER_H_

#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>  // XmbSetWMProperties()

#include "common/debug_IFrameBuffer.h"

class debug_XFrameBuffer : debug_IFrameBuffer
{
public:
    debug_XFrameBuffer(int w, int h)
        : m_width(w),
          m_height(h),
          m_isWindowReady(false),
          m_pXimage(0),
          m_pDisplay(0),
          m_pFrameBuffer(0)
    {
    }

    ~debug_XFrameBuffer()
    {
        fin();
    }

    void init()
    {
        if (XInitThreads() == 0)
            VERIFY(0);

        if ((m_pDisplay = XOpenDisplay(0)) == 0)
            VERIFY(0);

        m_window = XCreateSimpleWindow(m_pDisplay, DefaultRootWindow(m_pDisplay),
                                       0, 0, m_width, m_height, 3, WhitePixel(m_pDisplay, 0), BlackPixel(m_pDisplay, 0));

        if(m_window == BadAlloc || m_window == BadMatch || m_window == BadValue || m_window == BadWindow)
        {
            fin();
            VERIFY(0);
        }
        m_isWindowReady = true;

        XmbSetWMProperties(m_pDisplay, m_window, "Simple window", 0, 0, 0, 0, 0, 0);

        // XSelectInput(s_dpy, s_window, ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask);
        XMapWindow(m_pDisplay, m_window);
        XFlush(m_pDisplay);

        if (!(m_pFrameBuffer = (uint8_t*)malloc(m_width * m_height * m_bpp)))
        {
            fin();
            VERIFY(0);
        }

        if (!(m_pXimage = XCreateImage(m_pDisplay, CopyFromParent, DefaultDepth(m_pDisplay, DefaultScreen(m_pDisplay)),
                                       ZPixmap, 0, (char*)m_pFrameBuffer, m_width, m_height, m_bpp * 8, m_bpp * m_width)))
        {
            fin();
            VERIFY(0);
        }
    
        XGCValues xgcv;
        m_gc = XCreateGC(m_pDisplay, m_window, 0, &xgcv);
        XFlush(m_pDisplay);
        XSync(m_pDisplay, False);
    }

    void fin()
    {
        if (m_pXimage)
        {
            XFree(m_pXimage);
            m_pXimage = 0;
        }

        free(m_pFrameBuffer);
        m_pFrameBuffer = 0;

        if (m_pDisplay)
        {
            if (m_isWindowReady)
                XDestroyWindow(m_pDisplay, m_window);

            XCloseDisplay(m_pDisplay);
            m_pDisplay = 0;
            m_isWindowReady = false;
        }
    }

    void flip()
    {
        XPutImage(m_pDisplay, m_window, m_gc, m_pXimage, 0, 0, 0, 0, m_width, m_height);
        XFlush(m_pDisplay);
        XSync(m_pDisplay, False);
    }

    debug_FrameBufferPixel pos(int x, int y)
    {
        x = std::min(std::max(x, 0), m_width - 1);
        y = std::min(std::max(y, 0), m_height - 1);

        uint8_t* ptr = m_pFrameBuffer + (y * m_width + x) * m_bpp;

        return debug_FrameBufferPixel(ptr + 2, ptr + 1, ptr + 0, 0, 0);
    }

    int width() const { return m_width; }
    int height() const { return m_height; }
    int bpp() const { return m_bpp; }

private:
    static const int m_bpp = 4;

    int m_width;
    int m_height;

    Window m_window;
    bool m_isWindowReady;

    XImage* m_pXimage;
    GC m_gc;
    Display* m_pDisplay;
    uint8_t* m_pFrameBuffer;
};

#endif // DEBUG_X_FRAME_BUFFER_H_
