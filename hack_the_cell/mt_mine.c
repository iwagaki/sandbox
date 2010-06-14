#include <stdio.h>
#include <spu_mfcio.h>
#include "mt_mine.h"

/* Period parameters */
#define N 624
#define M 397

static unsigned long mt[N + 4] __attribute__((aligned(16))); 
static unsigned long mt_rnd[N] __attribute__((aligned(16))); 
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializes mt[N] with a seed */
void init_genrand_mine(unsigned long s)
{
  mt[0]= s & 0xffffffffUL;
  for (mti=1; mti<N; mti++) {
    mt[mti] =
      (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
    /* 2002/01/09 modified by Makoto Matsumoto             */
    mt[mti] &= 0xffffffffUL;
    /* for >32 bit machines */
  }
}

/* generate original mt[] and mt_rnd[] of random values after tempering */
void mt_generator_and_tempering()
{
    register int i;

    register vec_uint4* px;
    register vec_uint4* psx;
    register vec_uint4* psr;
    register vec_uint4* py;

    register const vec_uchar16 align_mask = {0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13};
    register const vec_uint4 sel_mask = spu_splats(0x80000000);
    register const vec_uint4 mag1 = spu_splats(0x9908b0df); 
    register const vec_uint4 and_mask1 = spu_splats(0x9d2c5680);
    register const vec_uint4 and_mask2 = spu_splats(0xefc60000);
    register const vec_uint4 vzero = (vec_uint4)spu_splats(0x0);

    register vec_uint4 x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
    register vec_uint4 y0, y1, y2, y3, y4, y5, y6, y7, y8, y9;

    register vec_uint4 v10, v11, v12, v13, v14, v15, v16, v17, v18;
    register vec_uint4 v20, v21, v22, v23, v24, v25, v26, v27, v28;
    register vec_uint4 v30, v31, v32, v33, v34, v35, v36, v37, v38;
    register vec_uint4 v40, v41, v42, v43, v44, v45, v46, v47, v48;
    register vec_uint4 v50, v51, v52, v53, v54, v55, v56, v57, v58;

    register vec_uint4 vsum0 = (vec_uint4)spu_splats(0x0);
    register vec_uint4 vsum1 = (vec_uint4)spu_splats(0x0);
    register vec_uint4 vsum2 = (vec_uint4)spu_splats(0x0);
    register vec_uint4 vsum3 = (vec_uint4)spu_splats(0x0);
    register vec_uint4 vsum4 = (vec_uint4)spu_splats(0x0);

    px = (vec_uint4*)mt;
    psx = (vec_uint4*)mt;
    psr = (vec_uint4*)mt_rnd;
    py = (vec_uint4*)mt + 99;

    x0 = *px++;
    x1 = *px++;
    y0 = *py++;
    y1 = *py++;

    /* kk = [0, 3] */
    {
        v20 = spu_shuffle(x0, x1, align_mask);
        v30 = spu_shuffle(y0, y1, align_mask);
        v10 = spu_sel(v20, x0, sel_mask);
        v10 = spu_xor(spu_rlmask(v10, -1), v30);
        v10 = spu_xor(v10, spu_andc(mag1, spu_subx(vzero, vzero, v20)));
        *psx++ = v10;

        *((vec_uint4*)mt + 156) = v10; /* mt[624] = mt[0] */

        v10 = spu_xor(v10, spu_rlmask(v10, -11));
        v10 = spu_xor(v10, spu_and(spu_slqw(v10, 7), and_mask1));
        v10 = spu_xor(v10, spu_and(spu_slqw(spu_slqwbyte(v10, 1), 7), and_mask2));
        v10 = spu_xor(v10, spu_rlmask(v10, -18));
        *psr++ = v10;
    }

    x0 = x1;
    x1 = *px++;
    x2 = *px++;
    x3 = *px++;
    x4 = *px++;
    x5 = *px++;
    x6 = *px++;
    x7 = *px++;
    x8 = *px++;

    y0 = y1;
    y1 = *py++;
    y2 = *py++;
    y3 = *py++;
    y4 = *py++;
    y5 = *py++;
    y6 = *py++;
    y7 = *py++;
    y8 = *py++;

    /* kk = [4, 227] */
    for (i = 0; i < 56 / 8; ++i)
    {
        v20 = spu_shuffle(x0, x1, align_mask);
        v21 = spu_shuffle(x1, x2, align_mask);
        v22 = spu_shuffle(x2, x3, align_mask);
        v23 = spu_shuffle(x3, x4, align_mask);
        v24 = spu_shuffle(x4, x5, align_mask);
        v25 = spu_shuffle(x5, x6, align_mask);
        v26 = spu_shuffle(x6, x7, align_mask);
        v27 = spu_shuffle(x7, x8, align_mask);

        v30 = spu_shuffle(y0, y1, align_mask);
        v31 = spu_shuffle(y1, y2, align_mask);
        v32 = spu_shuffle(y2, y3, align_mask);
        v33 = spu_shuffle(y3, y4, align_mask);
        v34 = spu_shuffle(y4, y5, align_mask);
        v35 = spu_shuffle(y5, y6, align_mask);
        v36 = spu_shuffle(y6, y7, align_mask);
        v37 = spu_shuffle(y7, y8, align_mask);

        v10 = spu_sel(v20, x0, sel_mask);
        v11 = spu_sel(v21, x1, sel_mask);
        v12 = spu_sel(v22, x2, sel_mask);
        v13 = spu_sel(v23, x3, sel_mask);
        v14 = spu_sel(v24, x4, sel_mask);
        v15 = spu_sel(v25, x5, sel_mask);
        v16 = spu_sel(v26, x6, sel_mask);
        v17 = spu_sel(v27, x7, sel_mask);

        v10 = spu_rlmask(v10, -1);
        v11 = spu_rlmask(v11, -1);
        v12 = spu_rlmask(v12, -1);
        v13 = spu_rlmask(v13, -1);
        v14 = spu_rlmask(v14, -1);
        v15 = spu_rlmask(v15, -1);
        v16 = spu_rlmask(v16, -1);
        v17 = spu_rlmask(v17, -1);

        v10 = spu_xor(v10, v30);
        v11 = spu_xor(v11, v31);
        v12 = spu_xor(v12, v32);
        v13 = spu_xor(v13, v33);
        v14 = spu_xor(v14, v34);
        v15 = spu_xor(v15, v35);
        v16 = spu_xor(v16, v36);
        v17 = spu_xor(v17, v37);

        v20 = spu_gather(v20);
        v21 = spu_gather(v21);
        v22 = spu_gather(v22);
        v23 = spu_gather(v23);
        v24 = spu_gather(v24);
        v25 = spu_gather(v25);
        v26 = spu_gather(v26);
        v27 = spu_gather(v27);

        v20 = spu_maskw(spu_extract(v20, 0));
        v21 = spu_maskw(spu_extract(v21, 0));
        v22 = spu_maskw(spu_extract(v22, 0));
        v23 = spu_maskw(spu_extract(v23, 0));
        v24 = spu_maskw(spu_extract(v24, 0));
        v25 = spu_maskw(spu_extract(v25, 0));
        v26 = spu_maskw(spu_extract(v26, 0));
        v27 = spu_maskw(spu_extract(v27, 0));

        v20 = spu_and(mag1, v20);
        v21 = spu_and(mag1, v21);
        v22 = spu_and(mag1, v22);
        v23 = spu_and(mag1, v23);
        v24 = spu_and(mag1, v24);
        v25 = spu_and(mag1, v25);
        v26 = spu_and(mag1, v26);
        v27 = spu_and(mag1, v27);

        v10 = spu_xor(v10, v20);
        v11 = spu_xor(v11, v21);
        v12 = spu_xor(v12, v22);
        v13 = spu_xor(v13, v23);
        v14 = spu_xor(v14, v24);
        v15 = spu_xor(v15, v25);
        v16 = spu_xor(v16, v26);
        v17 = spu_xor(v17, v27);

        *psx++ = v10;
        *psx++ = v11;
        *psx++ = v12;
        *psx++ = v13;
        *psx++ = v14;
        *psx++ = v15;
        *psx++ = v16;
        *psx++ = v17;

        x0 = x8;
        x1 = *px++;
        x2 = *px++;
        x3 = *px++;
        x4 = *px++;
        x5 = *px++;
        x6 = *px++;
        x7 = *px++;
        x8 = *px++;

        y0 = y8;
        y1 = *py++;
        y2 = *py++;
        y3 = *py++;
        y4 = *py++;
        y5 = *py++;
        y6 = *py++;
        y7 = *py++;
        y8 = *py++;

        v40 = spu_rlmask(v10, -11);
        v41 = spu_rlmask(v11, -11);
        v42 = spu_rlmask(v12, -11);
        v43 = spu_rlmask(v13, -11);
        v44 = spu_rlmask(v14, -11);
        v45 = spu_rlmask(v15, -11);
        v46 = spu_rlmask(v16, -11);
        v47 = spu_rlmask(v17, -11);

        v40 = spu_xor(v10, v40);
        v41 = spu_xor(v11, v41);
        v42 = spu_xor(v12, v42);
        v43 = spu_xor(v13, v43);
        v44 = spu_xor(v14, v44);
        v45 = spu_xor(v15, v45);
        v46 = spu_xor(v16, v46);
        v47 = spu_xor(v17, v47);

        v50 = spu_slqw(v40, 7);
        v51 = spu_slqw(v41, 7);
        v52 = spu_slqw(v42, 7);
        v53 = spu_slqw(v43, 7);
        v54 = spu_slqw(v44, 7);
        v55 = spu_slqw(v45, 7);
        v56 = spu_slqw(v46, 7);
        v57 = spu_slqw(v47, 7);

        v50 = spu_and(v50, and_mask1);
        v51 = spu_and(v51, and_mask1);
        v52 = spu_and(v52, and_mask1);
        v53 = spu_and(v53, and_mask1);
        v54 = spu_and(v54, and_mask1);
        v55 = spu_and(v55, and_mask1);
        v56 = spu_and(v56, and_mask1);
        v57 = spu_and(v57, and_mask1);

        v40 = spu_xor(v40, v50);
        v41 = spu_xor(v41, v51);
        v42 = spu_xor(v42, v52);
        v43 = spu_xor(v43, v53);
        v44 = spu_xor(v44, v54);
        v45 = spu_xor(v45, v55);
        v46 = spu_xor(v46, v56);
        v47 = spu_xor(v47, v57);

        v50 = spu_slqwbyte(v40, 1);
        v51 = spu_slqwbyte(v41, 1);
        v52 = spu_slqwbyte(v42, 1);
        v53 = spu_slqwbyte(v43, 1);
        v54 = spu_slqwbyte(v44, 1);
        v55 = spu_slqwbyte(v45, 1);
        v56 = spu_slqwbyte(v46, 1);
        v57 = spu_slqwbyte(v47, 1);

        v50 = spu_slqw(v50, 7);
        v51 = spu_slqw(v51, 7);
        v52 = spu_slqw(v52, 7);
        v53 = spu_slqw(v53, 7);
        v54 = spu_slqw(v54, 7);
        v55 = spu_slqw(v55, 7);
        v56 = spu_slqw(v56, 7);
        v57 = spu_slqw(v57, 7);

        v50 = spu_and(v50, and_mask2);
        v51 = spu_and(v51, and_mask2);
        v52 = spu_and(v52, and_mask2);
        v53 = spu_and(v53, and_mask2);
        v54 = spu_and(v54, and_mask2);
        v55 = spu_and(v55, and_mask2);
        v56 = spu_and(v56, and_mask2);
        v57 = spu_and(v57, and_mask2);

        v40 = spu_xor(v40, v50);
        v41 = spu_xor(v41, v51);
        v42 = spu_xor(v42, v52);
        v43 = spu_xor(v43, v53);
        v44 = spu_xor(v44, v54);
        v45 = spu_xor(v45, v55);
        v46 = spu_xor(v46, v56);
        v47 = spu_xor(v47, v57);

        v50 = spu_rlmask(v40, -18);
        v51 = spu_rlmask(v41, -18);
        v52 = spu_rlmask(v42, -18);
        v53 = spu_rlmask(v43, -18);
        v54 = spu_rlmask(v44, -18);
        v55 = spu_rlmask(v45, -18);
        v56 = spu_rlmask(v46, -18);
        v57 = spu_rlmask(v47, -18);

        v40 = spu_xor(v40, v50);
        v41 = spu_xor(v41, v51);
        v42 = spu_xor(v42, v52);
        v43 = spu_xor(v43, v53);
        v44 = spu_xor(v44, v54);
        v45 = spu_xor(v45, v55);
        v46 = spu_xor(v46, v56);
        v47 = spu_xor(v47, v57);

        *psr++ = v40;
        *psr++ = v41;
        *psr++ = v42;
        *psr++ = v43;
        *psr++ = v44;
        *psr++ = v45;
        *psr++ = v46;
        *psr++ = v47;
    }

    py = (vec_uint4*)mt;

    x9 = *px++;

    y0 = *py++;
    y1 = *py++;
    y2 = *py++;
    y3 = *py++;
    y4 = *py++;
    y5 = *py++;
    y6 = *py++;
    y7 = *py++;
    y8 = *py++;
    y9 = *py++;

    /* kk = [228, 623] */
    for (i = 0; i < 99 / 9; ++i)
    {
        v20 = spu_shuffle(x0, x1, align_mask);
        v21 = spu_shuffle(x1, x2, align_mask);
        v22 = spu_shuffle(x2, x3, align_mask);
        v23 = spu_shuffle(x3, x4, align_mask);
        v24 = spu_shuffle(x4, x5, align_mask);
        v25 = spu_shuffle(x5, x6, align_mask);
        v26 = spu_shuffle(x6, x7, align_mask);
        v27 = spu_shuffle(x7, x8, align_mask);
        v28 = spu_shuffle(x8, x9, align_mask);

        v30 = spu_shuffle(y0, y1, align_mask);
        v31 = spu_shuffle(y1, y2, align_mask);
        v32 = spu_shuffle(y2, y3, align_mask);
        v33 = spu_shuffle(y3, y4, align_mask);
        v34 = spu_shuffle(y4, y5, align_mask);
        v35 = spu_shuffle(y5, y6, align_mask);
        v36 = spu_shuffle(y6, y7, align_mask);
        v37 = spu_shuffle(y7, y8, align_mask);
        v38 = spu_shuffle(y8, y9, align_mask);

        v10 = spu_sel(v20, x0, sel_mask);
        v11 = spu_sel(v21, x1, sel_mask);
        v12 = spu_sel(v22, x2, sel_mask);
        v13 = spu_sel(v23, x3, sel_mask);
        v14 = spu_sel(v24, x4, sel_mask);
        v15 = spu_sel(v25, x5, sel_mask);
        v16 = spu_sel(v26, x6, sel_mask);
        v17 = spu_sel(v27, x7, sel_mask);
        v18 = spu_sel(v28, x8, sel_mask);

        v10 = spu_rlmask(v10, -1);
        v11 = spu_rlmask(v11, -1);
        v12 = spu_rlmask(v12, -1);
        v13 = spu_rlmask(v13, -1);
        v14 = spu_rlmask(v14, -1);
        v15 = spu_rlmask(v15, -1);
        v16 = spu_rlmask(v16, -1);
        v17 = spu_rlmask(v17, -1);
        v18 = spu_rlmask(v18, -1);

        v10 = spu_xor(v10, v30);
        v11 = spu_xor(v11, v31);
        v12 = spu_xor(v12, v32);
        v13 = spu_xor(v13, v33);
        v14 = spu_xor(v14, v34);
        v15 = spu_xor(v15, v35);
        v16 = spu_xor(v16, v36);
        v17 = spu_xor(v17, v37);
        v18 = spu_xor(v18, v38);

        v20 = spu_gather(v20);
        v21 = spu_gather(v21);
        v22 = spu_gather(v22);
        v23 = spu_gather(v23);
        v24 = spu_gather(v24);
        v25 = spu_gather(v25);
        v26 = spu_gather(v26);
        v27 = spu_gather(v27);
        v28 = spu_gather(v28);

        v20 = spu_maskw(spu_extract(v20, 0));
        v21 = spu_maskw(spu_extract(v21, 0));
        v22 = spu_maskw(spu_extract(v22, 0));
        v23 = spu_maskw(spu_extract(v23, 0));
        v24 = spu_maskw(spu_extract(v24, 0));
        v25 = spu_maskw(spu_extract(v25, 0));
        v26 = spu_maskw(spu_extract(v26, 0));
        v27 = spu_maskw(spu_extract(v27, 0));
        v28 = spu_maskw(spu_extract(v28, 0));

        v20 = spu_and(mag1, v20);
        v21 = spu_and(mag1, v21);
        v22 = spu_and(mag1, v22);
        v23 = spu_and(mag1, v23);
        v24 = spu_and(mag1, v24);
        v25 = spu_and(mag1, v25);
        v26 = spu_and(mag1, v26);
        v27 = spu_and(mag1, v27);
        v28 = spu_and(mag1, v28);

        v10 = spu_xor(v10, v20);
        v11 = spu_xor(v11, v21);
        v12 = spu_xor(v12, v22);
        v13 = spu_xor(v13, v23);
        v14 = spu_xor(v14, v24);
        v15 = spu_xor(v15, v25);
        v16 = spu_xor(v16, v26);
        v17 = spu_xor(v17, v27);
        v18 = spu_xor(v18, v28);

        *psx++ = v10;
        *psx++ = v11;
        *psx++ = v12;
        *psx++ = v13;
        *psx++ = v14;
        *psx++ = v15;
        *psx++ = v16;
        *psx++ = v17;
        *psx++ = v18;

        x0 = x9;
        x1 = *px++;
        x2 = *px++;
        x3 = *px++;
        x4 = *px++;
        x5 = *px++;
        x6 = *px++;
        x7 = *px++;
        x8 = *px++;
        x9 = *px++;

        y0 = y9;
        y1 = *py++;
        y2 = *py++;
        y3 = *py++;
        y4 = *py++;
        y5 = *py++;
        y6 = *py++;
        y7 = *py++;
        y8 = *py++;
        y9 = *py++;

        v40 = spu_rlmask(v10, -11);
        v41 = spu_rlmask(v11, -11);
        v42 = spu_rlmask(v12, -11);
        v43 = spu_rlmask(v13, -11);
        v44 = spu_rlmask(v14, -11);
        v45 = spu_rlmask(v15, -11);
        v46 = spu_rlmask(v16, -11);
        v47 = spu_rlmask(v17, -11);
        v48 = spu_rlmask(v18, -11);

        v40 = spu_xor(v10, v40);
        v41 = spu_xor(v11, v41);
        v42 = spu_xor(v12, v42);
        v43 = spu_xor(v13, v43);
        v44 = spu_xor(v14, v44);
        v45 = spu_xor(v15, v45);
        v46 = spu_xor(v16, v46);
        v47 = spu_xor(v17, v47);
        v48 = spu_xor(v18, v48);

        v50 = spu_slqw(v40, 7);
        v51 = spu_slqw(v41, 7);
        v52 = spu_slqw(v42, 7);
        v53 = spu_slqw(v43, 7);
        v54 = spu_slqw(v44, 7);
        v55 = spu_slqw(v45, 7);
        v56 = spu_slqw(v46, 7);
        v57 = spu_slqw(v47, 7);
        v58 = spu_slqw(v48, 7);

        v50 = spu_and(v50, and_mask1);
        v51 = spu_and(v51, and_mask1);
        v52 = spu_and(v52, and_mask1);
        v53 = spu_and(v53, and_mask1);
        v54 = spu_and(v54, and_mask1);
        v55 = spu_and(v55, and_mask1);
        v56 = spu_and(v56, and_mask1);
        v57 = spu_and(v57, and_mask1);
        v58 = spu_and(v58, and_mask1);

        v40 = spu_xor(v40, v50);
        v41 = spu_xor(v41, v51);
        v42 = spu_xor(v42, v52);
        v43 = spu_xor(v43, v53);
        v44 = spu_xor(v44, v54);
        v45 = spu_xor(v45, v55);
        v46 = spu_xor(v46, v56);
        v47 = spu_xor(v47, v57);
        v48 = spu_xor(v48, v58);

        v50 = spu_slqwbyte(v40, 1);
        v51 = spu_slqwbyte(v41, 1);
        v52 = spu_slqwbyte(v42, 1);
        v53 = spu_slqwbyte(v43, 1);
        v54 = spu_slqwbyte(v44, 1);
        v55 = spu_slqwbyte(v45, 1);
        v56 = spu_slqwbyte(v46, 1);
        v57 = spu_slqwbyte(v47, 1);
        v58 = spu_slqwbyte(v48, 1);

        v50 = spu_slqw(v50, 7);
        v51 = spu_slqw(v51, 7);
        v52 = spu_slqw(v52, 7);
        v53 = spu_slqw(v53, 7);
        v54 = spu_slqw(v54, 7);
        v55 = spu_slqw(v55, 7);
        v56 = spu_slqw(v56, 7);
        v57 = spu_slqw(v57, 7);
        v58 = spu_slqw(v58, 7);

        v50 = spu_and(v50, and_mask2);
        v51 = spu_and(v51, and_mask2);
        v52 = spu_and(v52, and_mask2);
        v53 = spu_and(v53, and_mask2);
        v54 = spu_and(v54, and_mask2);
        v55 = spu_and(v55, and_mask2);
        v56 = spu_and(v56, and_mask2);
        v57 = spu_and(v57, and_mask2);
        v58 = spu_and(v58, and_mask2);

        v40 = spu_xor(v40, v50);
        v41 = spu_xor(v41, v51);
        v42 = spu_xor(v42, v52);
        v43 = spu_xor(v43, v53);
        v44 = spu_xor(v44, v54);
        v45 = spu_xor(v45, v55);
        v46 = spu_xor(v46, v56);
        v47 = spu_xor(v47, v57);
        v48 = spu_xor(v48, v58);

        v50 = spu_rlmask(v40, -18);
        v51 = spu_rlmask(v41, -18);
        v52 = spu_rlmask(v42, -18);
        v53 = spu_rlmask(v43, -18);
        v54 = spu_rlmask(v44, -18);
        v55 = spu_rlmask(v45, -18);
        v56 = spu_rlmask(v46, -18);
        v57 = spu_rlmask(v47, -18);
        v58 = spu_rlmask(v48, -18);

        v40 = spu_xor(v40, v50);
        v41 = spu_xor(v41, v51);
        v42 = spu_xor(v42, v52);
        v43 = spu_xor(v43, v53);
        v44 = spu_xor(v44, v54);
        v45 = spu_xor(v45, v55);
        v46 = spu_xor(v46, v56);
        v47 = spu_xor(v47, v57);
        v48 = spu_xor(v48, v58);

        *psr++ = v40;
        *psr++ = v41;
        *psr++ = v42;
        *psr++ = v43;
        *psr++ = v44;
        *psr++ = v45;
        *psr++ = v46;
        *psr++ = v47;
        *psr++ = v48;
    }

    mti = 0;
}


unsigned int
genrand_mine(int num_rand)
{
    /* assume that mti to be 0 */ 

    register int count = num_rand;

    register int i;
    register vec_uint4 vsum = (vec_uint4)spu_splats(0x0);

    {
        register vec_uint4 vsum0 = (vec_uint4)spu_splats(0x0);
        register vec_uint4 vsum1 = (vec_uint4)spu_splats(0x0);
        register vec_uint4 vsum2 = (vec_uint4)spu_splats(0x0);
        register vec_uint4 vsum3 = (vec_uint4)spu_splats(0x0);
        register vec_uint4 vsum4 = (vec_uint4)spu_splats(0x0);
        register vec_uint4 vsum5 = (vec_uint4)spu_splats(0x0);

        /* get sum of 624 * n random values */
        do
        {
            register vec_uint4 r0, r1, r2, r3, r4, r5;
            register vec_uint4* prnd = (vec_uint4*)mt_rnd;

            mt_generator_and_tempering(); /* generate mt table and store mt_rnd table */

            r0 = *prnd++;
            r1 = *prnd++;
            r2 = *prnd++;
            r3 = *prnd++;
            r4 = *prnd++;
            r5 = *prnd++;

            for (i = 0; i < 624 / 4 / 6; ++i)
            {
                vsum0 = spu_add(vsum0, r0);
                vsum1 = spu_add(vsum1, r1);
                vsum2 = spu_add(vsum2, r2);
                vsum3 = spu_add(vsum3, r3);
                vsum4 = spu_add(vsum4, r4);
                vsum5 = spu_add(vsum5, r5);

                r0 = *prnd++;
                r1 = *prnd++;
                r2 = *prnd++;
                r3 = *prnd++;
                r4 = *prnd++;
                r5 = *prnd++;
            }
            
            count -= 624;
        }
        while (count > 624); /* assume that num_rand >= 624 */

        vsum = spu_add(vsum, vsum0);
        vsum = spu_add(vsum, vsum1);
        vsum = spu_add(vsum, vsum2);
        vsum = spu_add(vsum, vsum3);
        vsum = spu_add(vsum, vsum4);
        vsum = spu_add(vsum, vsum5);
    }

    /* get sum of the rest of random values */
    {
        register vec_uint4* prnd = (vec_uint4*)mt_rnd;

        mt_generator_and_tempering(); /* generate mt table and store mt_rnd table */

        /* assume that num_rand % 4 == 0 */
        while (count >= 4)
        {
            vsum = spu_add(vsum, *prnd++);
            count -= 4;
        }
    }

    return spu_extract(vsum, 0) + spu_extract(vsum, 1) + spu_extract(vsum, 2) + spu_extract(vsum, 3); 
}
