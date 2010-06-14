#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <algorithm>

#include "common/FileDescriptor.h"
#include "common/OptionParserMain.h"
#include "common/debug_XFrameBuffer.h"
#include "common/debug_WindowsBitmapBuffer.h"
#include "common/debug_GrayscaleFrameBuffer.h"

//#define ORIGINAL

double&      g_alpha       = OptionParser::createOption(1.0, "alpha", "alpha");
double&      g_alpha2      = OptionParser::createOption(1.0, "alpha2", "alpha2");
double&      g_beta        = OptionParser::createOption(10000.0, "beta", "beta");
double&      g_gamma       = OptionParser::createOption(20.0, "gamma", "gamma");
bool&        g_is_original = OptionParser::createOption(false, "original", "original mode");
const char*& g_image_file  = OptionParser::createOption("lena_gray_small.bmp", "i", "image file");


template <typename T> double delta(T i, T j) {
    return i == j ? 1.0 : 0.0;
}


inline double prior1(debug_IFrameBuffer& h, debug_IFrameBuffer& v, int x, int y) {
    return delta(h.pos(x, y).v() + h.pos(x - 1, y).v(), 2) + delta(v.pos(x, y).v() + v.pos(x, y - 1).v(), 2);
}

inline double prior2(debug_IFrameBuffer& h, debug_IFrameBuffer& v, int x, int y) {
    return (1 - delta(v.pos(x, y).v() + h.pos(x, y).v() + v.pos(x + 1, y).v() + h.pos(x, y + 1).v(), 2));
}


double prior_step3(debug_IFrameBuffer& h, debug_IFrameBuffer& v, double beta, int cx, int cy) {
    double sum1 = 0.0, sum2 = 0.0;

    sum1 += prior1(h, v, cx, cy);
    sum1 += prior1(h, v, cx + 1, cy);
    sum1 += prior1(h, v, cx, cy + 1);
    sum1 += prior1(h, v, cx - 1, cy);
    sum1 += prior1(h, v, cx, cy - 1);

    sum2 += prior2(h, v, cx, cy);
    sum2 += prior2(h, v, cx - 1, cy);
    sum2 += prior2(h, v, cx, cy - 1);
    sum2 += prior2(h, v, cx - 1, cy - 1);

    return - beta * (sum1 + sum2);
}


double prior_step4(debug_IFrameBuffer& h, debug_IFrameBuffer& v, double beta, int cx, int cy) {
    double sum1 = 0.0, sum2 = 0.0;

    sum1 += prior1(h, v, cx, cy);
    sum1 += prior1(h, v, cx + 1, cy);
    sum1 += prior1(h, v, cx, cy + 1);
    sum1 += prior1(h, v, cx + 1, cy + 1);

    sum2 += prior2(h, v, cx, cy);
    sum2 += prior2(h, v, cx + 1, cy);
    sum2 += prior2(h, v, cx, cy + 1);
    sum2 += prior2(h, v, cx - 1, cy);
    sum2 += prior2(h, v, cx, cy - 1);

    return - beta * (sum1 + sum2);
}


double prob_h(debug_IFrameBuffer& f, debug_IFrameBuffer& h, double alpha, double alpha2, double gamma, int x, int y) {
    double df = static_cast<double>(f.pos(x, y).r() - f.pos(x + 1, y).r());
    double d = df * df - gamma * gamma;

    if (g_is_original)
        return alpha * (1 - h.pos(x, y).v()) * d;
    else
        return (d > 0.0 ? alpha : alpha2) * (1 - h.pos(x, y).v()) * d;
}

double prob_v(debug_IFrameBuffer& f, debug_IFrameBuffer& v, double alpha, double alpha2, double gamma, int x, int y) {
    double df = static_cast<double>(f.pos(x, y).r() - f.pos(x, y + 1).r());
    double d = df * df - gamma * gamma;

    if (g_is_original)
        return alpha * (1 - v.pos(x, y).v()) * d;
    else
        return (d > 0.0 ? alpha : alpha2) * (1 - v.pos(x, y).v()) * d;
}

double prob_step3(debug_IFrameBuffer& f, debug_IFrameBuffer& h, debug_IFrameBuffer& v, double alpha, double alpha2, double gamma, int cx, int cy) {
    double sum = 0.0;

    sum += prob_h(f, h, alpha, alpha2, gamma, cx, cy);
    sum += prob_h(f, h, alpha, alpha2, gamma, cx - 1, cy);

    sum += prob_v(f, v, alpha, alpha2, gamma, cx, cy);
    sum += prob_v(f, v, alpha, alpha2, gamma, cx, cy - 1);

    return - sum;
}

double prob_step4(debug_IFrameBuffer& f, debug_IFrameBuffer& h, debug_IFrameBuffer& v, double alpha, double alpha2, double gamma, int cx, int cy) {
    double sum = 0.0;

    sum += prob_h(f, h, alpha, alpha2, gamma, cx, cy);
    sum += prob_h(f, h, alpha, alpha2, gamma, cx, cy + 1);

    sum += prob_v(f, v, alpha, alpha2, gamma, cx, cy);
    sum += prob_v(f, v, alpha, alpha2, gamma, cx + 1, cy);

    return - sum;
}


double posterior_step3(debug_IFrameBuffer& f, debug_IFrameBuffer& h, debug_IFrameBuffer& v, double alpha, double alpha2, double beta, double gamma, int cx, int cy) {
    return prior_step3(h, v, beta, cx, cy) + prob_step3(f, h, v, alpha, alpha2, gamma, cx, cy);
}

double posterior_step4(debug_IFrameBuffer& f, debug_IFrameBuffer& h, debug_IFrameBuffer& v, double alpha, double alpha2, double beta, double gamma, int cx, int cy) {
    return prior_step4(h, v, beta, cx, cy) + prob_step4(f, h, v, alpha, alpha2, gamma, cx, cy);
}


int main(int argc, char *argv[]) {
    OptionParser::parse(argc, argv);

    // load F image field
    FileLoader inputFile(g_image_file);
    debug_WindowsBitmapBuffer f(inputFile);

    printf("width = %d, height = %d, bpp = %d\n", f.width(), f.height(), f.bpp());

    // create H and V line field

    debug_GrayscaleFrameBuffer h(f.width() - 1, f.height() - 1, 1, new uint8_t[f.width() * f.height() * f.bpp()]);
    debug_GrayscaleFrameBuffer v(f.width() - 1, f.height() - 1, 1, new uint8_t[f.width() * f.height() * f.bpp()]);
    debug_GrayscaleFrameBuffer tmph(f.width() - 1, f.height() - 1, 1, new uint8_t[f.width() * f.height() * f.bpp()]);
    debug_GrayscaleFrameBuffer tmpv(f.width() - 1, f.height() - 1, 1, new uint8_t[f.width() * f.height() * f.bpp()]);

    debug_XFrameBuffer fb(f.width() * 2, f.height() * 2);

    fb.init();

    for (int y = 0; y < fb.height(); ++y)
        for (int x = 0; x < fb.width(); ++x)
            fb.pos(x, y).r() = fb.pos(x, y).g() = fb.pos(x, y).b() = 0;

    // ICM Step 1
    for (int y = 0; y < h.height(); ++y)
        for (int x = 0; x < h.width(); ++x)
            h.pos(x, y).v() = v.pos(x, y).v() = 0;

    for (;;) {

        // ICM Step 2
        for (int y = 0; y < h.height(); ++y) {
            for (int x = 0; x < h.width(); ++x) {
                tmph.pos(x, y).v() = h.pos(x, y).v();
                tmpv.pos(x, y).v() = v.pos(x, y).v();
            }
        }

        // ICM Step 3
        for (int y = 1; y < v.height(); ++y) {
            for (int x = 1; x < h.width(); ++x) {
                double p = 0.0;
                int argmaxr = -1;

                for (int r = 0; r < 16; ++r) {
                    h.pos(x, y).v() = (r & 0x1);
                    v.pos(x, y).v() = (r & 0x2) >> 1;
                    h.pos(x - 1, y).v() = (r & 0x4) >> 2;
                    v.pos(x, y - 1).v() = (r & 0x8) >> 3;

                    double pp = posterior_step3(f, h, v, g_alpha, g_alpha2, g_beta, g_gamma, x, y);

                    if ((argmaxr < 0) || (pp > p)) {
                        p = pp;
                        argmaxr = r;
                    }
                }

                h.pos(x, y).v() = (argmaxr & 0x1);
                v.pos(x, y).v() = (argmaxr & 0x2) >> 1;
                h.pos(x - 1, y).v() = (argmaxr & 0x4) >> 2;
                v.pos(x, y - 1).v() = (argmaxr & 0x8) >> 3;
            }
        }

        // ICM Step 4
        for (int y = 0; y < v.height() - 1; ++y) {
            for (int x = 0; x < h.width() - 1; ++x) {
                double p = 0.0;
                int argmaxr = -1;

                for (int r = 0; r < 16; ++r) {
                    h.pos(x, y).v() = (r & 0x1);
                    v.pos(x, y).v() = (r & 0x2) >> 1;
                    h.pos(x, y + 1).v() = (r & 0x4) >> 2;
                    v.pos(x + 1, y).v() = (r & 0x8) >> 3;

                    double pp = posterior_step4(f, h, v, g_alpha, g_alpha2, g_beta, g_gamma, x, y);

                    if ((argmaxr < 0) || (pp > p)) {
                        p = pp;
                        argmaxr = r;
                    }
                }

                h.pos(x, y).v() = (argmaxr & 0x1);
                v.pos(x, y).v() = (argmaxr & 0x2) >> 1;
                h.pos(x, y + 1).v() = (argmaxr & 0x4) >> 2;
                v.pos(x + 1, y).v() = (argmaxr & 0x8) >> 3;
            }
        }

        // ICM Step 5
        int sumdh = 0;
        int sumdv = 0;

        for (int y = 0; y < h.height(); ++y) {
            for (int x = 0; x < h.width(); ++x) {
                sumdh += abs(tmph.pos(x, y).v() - h.pos(x, y).v());
                sumdv += abs(tmpv.pos(x, y).v() - v.pos(x, y).v());
            }
        }

        printf("sumdh + sumdv = %d\n", sumdh + sumdv);

        // output H and V line field

        for (int y = 0; y < f.height() - 1; ++y) {
            for (int x = 0; x < f.width() - 1; ++x) {
                fb.pos(x * 2 + 1, y * 2).r() = h.pos(x, y).v() * 255;
                fb.pos(x * 2 + 1, y * 2).g() = 0;
                fb.pos(x * 2 + 1, y * 2).b() = 0;

                fb.pos(x * 2, y * 2 + 1).r() = v.pos(x, y).v() * 255;
                fb.pos(x * 2, y * 2 + 1).g() = 0;
                fb.pos(x * 2, y * 2 + 1).b() = 0;
            }
        }

        fb.flip();

        if (sumdh + sumdv < 100)
            break;
    }

    printf("press enter key\n");
    getchar();

    fb.fin();

    return 0;
}
