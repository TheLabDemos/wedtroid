
#include <cmath>
#include <memory.h>

#include "imaktgen.h"
#include "imakgp.h"
#include "icolor.h"
#include "3dmath.h"
#include "ilog.h"

void TextureData::build(UCHAR wexp, UCHAR hexp, USHORT frames, float fps)
{
    if (data_ != NULL)
        demolish();
    wexp_   = wexp;
    hexp_   = hexp;
    frames_ = frames;
    fps_    = fps;
    width_  = 1 << wexp_;
    height_ = 1 << hexp_;
    fsize_  = width_ * height_;
    noe_    = fsize_ * frames_;
    data_   = new IColor[noe_];
}


void TextureData::setAlphaAvg()
{
	for(int i = 0; i < noe_; i++)
	{
		data_[i].Alpha = ((short)data_[i].Red +
			              (short)data_[i].Green +
						  (short)data_[i].Blue) / 3;
	}
}


void TextureData::postMask(IColor col)
{
	
	float nred   = (float)(col.Red   ^ 255) /255.0f,
		  ngreen = (float)(col.Green ^ 255) /255.0f,
		  nblue  = (float)(col.Blue  ^ 255) /255.0f,
		  nalpha = (float)(col.Alpha ^ 255) /255.0f;

	IColor *ptr = data_;
	IColor *end = data_ + noe_;
	while (ptr < end)
	{
		ptr->Red   *= nred;
		ptr->Green *= ngreen;
		ptr->Blue  *= nblue;
		(*ptr++).Alpha *= nalpha;
	}
}


void TextureData::demolish()
{
    if (data_ != NULL)
    {
        delete [] data_;
        data_ = NULL;
    }
}


//void TextureData::fold()
//{
//    for (ULONG i = 0; i < noe_; i++)
//    {
//        data_[i].Red   = fabs((int)data_[i].Red   - 127.5f) * 2;
//        data_[i].Green = fabs((int)data_[i].Green - 127.5f) * 2;
//        data_[i].Blue  = fabs((int)data_[i].Blue  - 127.5f) * 2;
//        data_[i].Alpha = fabs((int)data_[i].Alpha - 127.5f) * 2;
//    }
//}


//void TextureData::invert()
//{
//    for (ULONG i = 0; i < noe_; i++)
//    {
//        data_[i].Red   = 255 - data_[i].Red;
//        data_[i].Green = 255 - data_[i].Green;
//        data_[i].Blue  = 255 - data_[i].Blue;
//        data_[i].Alpha = 255 - data_[i].Alpha;
//    }
//}


void TextureData::invert()
{
    for (ULONG i = 0; i < noe_; i++)
    {
        data_[i].Red   ^= 255;
        data_[i].Green ^= 255;
        data_[i].Blue  ^= 255;
        data_[i].Alpha ^= 255;
    }
}


void TextureData::envmap()
{
    float * tmp = new float[fsize_];
    int i, j, k;
    float jp, kp, hs = 2 / (float)height_, ws = 2 / (float)width_;
    for (j = 0, jp = -1.0f; j < height_; j++, jp+=hs)
    for (k = 0, kp = -1.0f; k < width_ ; k++, kp+=ws)
        tmp[j*width_+k] = 1.0f-sqrt(jp*jp+kp*kp);
    for (i = 0; i < fsize_; i++)
        if (tmp[i] < 0.0f) tmp[i] = 0.0f;
    for (i = 0; i < noe_; i++)
    {
        data_[i].Red   *= tmp[i%fsize_];
        data_[i].Green *= tmp[i%fsize_];
        data_[i].Blue  *= tmp[i%fsize_];
        data_[i].Alpha *= tmp[i%fsize_];
    }
    delete [] tmp;
}



void TextureData::setlayer(Layer layer, UCHAR value)
{
    for (ULONG i = 0; i < noe_; i++)
        ((UCHAR*)&data_[i])[layer] = value;
}


void TextureData::build(ScalarData &sd, IColor color1, IColor color2)
{
    this->build(log(2.0, sd.width_), log(2.0, sd.height_), sd.frames_, sd.fps_);
    for (ULONG il = 0; il < noe_; il++)
        data_[il] = colorMix(color1, color2, sd.data_[il]);
}


void ScalarData::build(UINT width, UINT height, USHORT frames, float fps)
{
    if (data_ != NULL)
        demolish();
    frames_ = frames;
    fps_    = fps;
    width_  = width;
    height_ = height;
    fsize_  = width_ * height_;
    noe_    = fsize_ * frames_;
    data_   = new float[noe_];
    h_idx_  = new int[height_];
    d_idx_  = new int[frames_];
    register int i;
    for (i = height_-1; i >= 0; i--)
        h_idx_[i] = i * width_;
    for (i = frames_-1; i >= 0; i--)
        d_idx_[i] = i * fsize_;
}



void ScalarData::slaughter(float upper_cleaver, float lower_cleaver)
{
    if (upper_cleaver > 1.0f) upper_cleaver = 1.0f;
    if (lower_cleaver < 0.0f) lower_cleaver = 0.0f;
    if (upper_cleaver != 1.0f)
        for (int i = 0; i < this->noe_; i++)
        {
            if (data_[i] > upper_cleaver) data_[i] = upper_cleaver;
        }
    if (lower_cleaver != 0.0f)
        for (int i = 0; i < this->noe_; i++)
        {
            if (data_[i] < lower_cleaver) data_[i] = lower_cleaver;
        }
}




void ScalarData::demolish()
{
    if (this->data_ != NULL)
    {
        delete [] data_;
        data_ = NULL;
        delete [] h_idx_;
        delete [] d_idx_;
    }
}


void ScalarData::clear()
{
    for (ULONG i = 0; i < noe_; i++)
        data_[i] = 0.0f;
}


void ScalarData::normalize()
{
    float fmin = 10.0f;
    ULONG i;
    for (i = 0; i < noe_; i++)
        if (data_[i] < fmin) fmin = data_[i];

    for (i = 0; i < noe_; i++)
        data_[i] -= fmin;

    float fmax = fmin;
    for (i = 0; i < noe_; i++)
        if (data_[i] > fmax) fmax = data_[i];

    for (i = 0; i < noe_; i++)
        data_[i] /= fmax;
}


//void ScalarData::normalize()
//{
//    float fmin = 10.0f;
//    float * td = data_;
//    UINT limit = noe_ * 4;
//    register UINT i;
//
//    for (i = 0; i < noe_; i++)
//        if (data_[i] < fmin) fmin = data_[i];
//
//    __asm
//    {
//        align   16
//        mov     ebx, 0
//        movss   xmm0, fmin
//        shufps  xmm0, xmm0, 0x00
//        mov     eax,  td
//        align   16
//llb1a:  movups  xmm1, [eax+ebx]
//        subps   xmm1, xmm0
//        movups  [eax+ebx], xmm1
//        add     ebx, 0x10
//        cmp     ebx, limit
//        ja      llb1a
//        sub     ebx, 0x10
//llb2a:  movss   xmm1, [eax+ebx]
//        subss   xmm1, xmm0
//        movss   [eax+ebx], xmm1
//        add     ebx, 4
//        cmp     ebx, limit
//        jne     llb2a
//    }
//
//    float fmax = fmin;
//    for (i = 0; i < noe_; i++)
//        if (data_[i] > fmax) fmax = data_[i];
//
//    __asm
//    {
//        align   16
//        mov     ebx, 0
//        movss   xmm0, fmax
//        shufps  xmm0, xmm0, 0x00
//        mov     eax,  td
//        align   16
//llb1b:  movups  xmm1, [eax+ebx]
//        divps   xmm1, xmm0
//        movups  [eax+ebx], xmm1
//        add     ebx, 0x10
//        cmp     ebx, limit
//        ja      llb1b
//        sub     ebx, 0x10
//llb2b:  movss   xmm1, [eax+ebx]
//        divss   xmm1, xmm0
//        movss   [eax+ebx], xmm1
//        add     ebx, 4
//        cmp     ebx, limit
//        jne     llb2b
//    }
//
//    for (i = 0; i < noe_; i++)
//        data_[i] /= fmax;
//}


void ScalarData::pntransform()
{
    for (UINT i = 0; i < noe_; i++)
        data_[i] = 2 * data_[i] - 1;
}


void ScalarData::ptransform()
{
    for (UINT i = 0; i < noe_; i++)
        data_[i] = data_[i] * 0.5 + 0.5;
}


void ScalarData::mult(float factor)
{
    for (UINT i = 0; i < noe_; i++)
        data_[i] *= factor;
}


void ScalarData::raise(float power)
{
    for (UINT i = 0; i < this->noe_; i++)
        data_[i] = pow(data_[i], power);
}


void ScalarData::abs()
{
    for (UINT i = 0; i < noe_; i++)
        data_[i] = fabs(data_[i] - 0.5f) * 2;
}
////Aoou a?iae oi bias oui Perlin eae Hoffert (1989). A?iae anau eae ic ooiiaoneeu.
//void ScalarData::bias(float b)
//{
//    for (UINT i = 0; i < noe_; i++)
//        data_[i] = __pow_baez(data_[i], log(0.5, b));
//}

//Aoo? a?iae oi aiaeeaeoeeu bias oio Christophe Schlick (1994). An?aini eae ooiiaoneeu.
void ScalarData::bias(float b)
{
    for (UINT i = 0; i < noe_; i++)
        data_[i] /= (1/b-2)*(1-data_[i])+1;
}

////Aoou a?iae oi gain oui Perlin eae Hoffert (1989). A?iae anau eae ic ooiiaoneeu.
//void ScalarData::gain(float g)
//{
//    for (UINT i = 0; i < noe_; i++)
//    {
//        if (data_[i] < 0.5)
//            data_[i] =      __pow_baez(  2*data_[i], log(0.5, 1-g))  * 0.5;
//        else
//            data_[i] = (2 - __pow_baez(2-2*data_[i], log(0.5, 1-g))) * 0.5;
//    }
//}

//Aoo? a?iae oi aiaeeaeoeeu gain oio Christophe Schlick (1994). An?aini eae ooiiaoneeu.
void ScalarData::gain(float g)
{
    for (UINT i = 0; i < noe_; i++)
    {
        register float fct = (1/g-2)*(1-2*data_[i]);
        if (data_[i] < 0.5)
            data_[i] /= fct+1;
        else
            data_[i] = (fct-data_[i])/(fct-1);
    }
}


void ScalarData::invert()
{
    for (ULONG i = 0; i < noe_; i++)
        data_[i] = 1 - data_[i];
}


void ScalarData::map(ScalarData &hf)
{
    if (this->noe_ == 0) return;
    int   v0i, v1i, v2i, v3i, h0i, h1i, h2i, h3i,
          i, j, k, m, v0t, v1t, v2t, v3t;
    float incx = (hf.width_/(float)width_),
          incy = (hf.height_/(float)height_),
          fcx, fcy, vpos, ntr0, ntr1, ntr2, ntr3;
    for (i = 0, fcy = 0.0f; i < height_; i++, fcy+=incy)
    {
        v0i = hf.height_-3+(v3i=1+(v2i=1+(v1i=(int)(fcy))));
        v0t = hf.width_ * (v0i %= hf.height_);
        v1t = hf.width_ *  v1i;
        v2t = hf.width_ * (v2i %= hf.height_);
        v3t = hf.width_ * (v3i %= hf.height_);
        vpos = fcy - v1i;
        for (j = 0, fcx = 0.0f; j < width_; j++, fcx+=incx)
        {
            h0i = hf.width_-3+(h3i=1+(h2i=1+(h1i=(int)(fcx))));
            h0i %= hf.width_;
            h2i %= hf.width_;
            h3i %= hf.width_;
            ntr0 = curp(hf.data_[v0t+h0i], hf.data_[v1t+h0i],
                        hf.data_[v2t+h0i], hf.data_[v3t+h0i], vpos);
            ntr1 = curp(hf.data_[v0t+h1i], hf.data_[v1t+h1i],
                        hf.data_[v2t+h1i], hf.data_[v3t+h1i], vpos);
            ntr2 = curp(hf.data_[v0t+h2i], hf.data_[v1t+h2i],
                        hf.data_[v2t+h2i], hf.data_[v3t+h2i], vpos);
            ntr3 = curp(hf.data_[v0t+h3i], hf.data_[v1t+h3i],
                        hf.data_[v2t+h3i], hf.data_[v3t+h3i], vpos);
            data_[i*width_+j] = curp(ntr0, ntr1, ntr2, ntr3, fcx-h1i);
        }
    }
}



void ScalarData::shift(ScalarData &hf, float xs, float ys, float zs)
{
    if (noe_ != hf.noe_) return;

    int i, j, k, c = 0;
    float * tdata = new float[noe_];

    for (i = 0; i < frames_; i++)
        for (j = 0; j < height_; j++)
            for (k = 0; k < width_; k++, c++)
                tdata[c] = data_[(int)(i + hf.data_[c] * zs * frames_) % frames_ * fsize_ +
                                 (int)(j + hf.data_[c] * ys * height_) % height_ * width_ +
                                 (int)(k + hf.data_[c] * xs * width_ ) % width_];
    delete [] data_;
    data_ = tdata;
}



void TextureGenerator::perlin(TextureData &td, UCHAR low, UCHAR high, UCHAR lacunarity,
                              UCHAR persistence, UCHAR rfi, UCHAR tides)
{
    ScalarData sd(td.width_, td.height_, td.frames_, td.fps_);
    this->perlin(sd, low, high, lacunarity, persistence, rfi, tides);
    //for (ULONG il = 0; il < td.noe_; il++)
    //    td.data_[il] = colorMix(this->color1, this->color2, sd.data_[il]);
    td.build(sd, color1, color2);
}


/*
void TextureGenerator::perlin(ScalarData &hf, UCHAR low, UCHAR high, UCHAR lacunarity,
                              UCHAR persistence, UCHAR rfi, UCHAR tides)
{
    float rlacu = (lacunarity < 5) ? float(1 << (lacunarity)) / 8: lacunarity - 2;
    float rpers = (float)persistence/(16);
    int iterations = 0;
    for (float fc = high; fc >= low; fc-=rlacu, iterations++);
    Noise ** nn = new Noise*[iterations];
    hf.clear();
    float cnt = (float)low;
    for (UCHAR uc = 0; uc < iterations; uc++, cnt+=rlacu)
    {
        int aratiow = hf.width_/hf.height_, aratioh = hf.height_/hf.width_,
            xmul, ymul, zmul;
        xmul = (aratiow > aratioh) ? ymul = 1, aratiow : ymul = aratioh, 1;
        float factor = pow(2, cnt);
        nn[uc] = new Noise(xmul*factor, ymul*factor, tides*factor);
    }
    float wrcp = 1 / (float)hf.width_, hrcp = 1 / (float)hf.height_,
          drcp = 1 / (float)hf.frames_,
          wcnt, hcnt, dcnt;
    int i, j, k, m;
    for (i = 0; i < iterations; rpers *= rpers, i++)
    {
        for (m = 0, dcnt = 0.0f; m < hf.frames_; m++, dcnt += drcp)
        for (k = 0, hcnt = 0.0f; k < hf.height_; k++, hcnt += hrcp)
        for (j = 0, wcnt = 0.0f; j < hf.width_ ; j++, wcnt += wrcp)
           hf.data_[m*hf.fsize_+k*hf.width_+j] += rpers * nn[i]->Get3D(wcnt, hcnt, dcnt);
           //hf.data_[m*hf.fsize_+k*hf.width_+j] += rpers * nn[i]->Get2D(wcnt, hcnt);
    }
    hf.normalize();
    for (i = 0; i < iterations; i++)
        delete nn[i];
    delete [] nn;
}
*/


void 
TextureGenerator::perlin(ScalarData &hf, UCHAR low, UCHAR high, UCHAR lacunarity,
                              UCHAR persistence, UCHAR rfi, UCHAR tides)
{
    float rlacu = (lacunarity < 5) ? float(1 << (lacunarity)) / 8: lacunarity - 2;
    float rpers = (float)persistence/(16);
    float wrcp = 1 / (float)hf.width_, hrcp = 1 / (float)hf.height_,
          drcp = 1 / (float)hf.frames_,
          wc, hc, dc, wcu, hcu, dcu, xi, yi, zi;
    int aratiow = hf.width_/hf.height_, aratioh = hf.height_/hf.width_,
        xmul, ymul, zmul;
    xmul = (aratiow > aratioh) ? ymul = 1, aratiow : ymul = aratioh, 1;
    int i, j, k, m;
    int iterations = 0;
    for (float fc = high; fc >= low; fc-=rlacu, iterations++);
    hf.clear();
    float cnt = (float)low;
    int i0, i1, i2, i3, j0, j1, j2, j3, k0, k1, k2, k3, i0o = -1;
    double r[64];



    for (i = 0; i < iterations; i++, cnt+=rlacu, rpers *= rpers)
    {
        float factor = pow(2, cnt);
        int d1 = xmul*factor, d2 = ymul*factor, d3 = tides*factor;
        for (m = 0, dcu = 0.0f; m < hf.frames_; m++, dcu += drcp)
        {
            dc  = dcu-(int)dcu;
            k1  = zi = d3 * dc;
            zi -= k1;
            k0  = bnd(k1-1, d3) * d1 * d2;
            k2  = bnd(k1+1, d3) * d1 * d2;
            k3  = bnd(k1+2, d3) * d1 * d2;
            k1  = bnd(k1,   d3) * d1 * d2;
            for (k = 0, hcu = 0.0f; k < hf.height_; k++, hcu += hrcp)
            {
                hc  = hcu-(int)hcu;
                j1  = yi = d2 * hc;
                yi -= j1;
                j0  = bnd(j1-1, d2) * d1;
                j2  = bnd(j1+1, d2) * d1;
                j3  = bnd(j1+2, d2) * d1;
                j1  = bnd(j1,   d2) * d1;
                //int k0j0 = k0+j0, k0j1 = k0+j1, k0j2 = k0+j2, k0j3 = k0+j3,
                //    k1j0 = k1+j0, k1j1 = k1+j1, k1j2 = k1+j2, k1j3 = k1+j3,
                //    k2j0 = k2+j0, k2j1 = k2+j1, k2j2 = k2+j2, k2j3 = k2+j3,
                //    k3j0 = k3+j0, k3j1 = k3+j1, k3j2 = k3+j2, k3j3 = k3+j3;
                for (j = 0, wcu = 0.0f; j < hf.width_ ; j++, i0o = i0, wcu+= wrcp)
                {
                    wc  = wcu-(int)wcu;
                    i1  = xi = d1 * wc;
                    xi -= i1;
                    i0  = bnd(i1-1, d1);
                    i1  = bnd(i1,   d1);
                    i2  = bnd(i1+1, d1);
                    i3  = bnd(i1+2, d1);

                    if(i0o != i0)
                    {
                        r[ 0]=fr(i0+j0+k0+rfi); r[ 1]=fr(i0+j0+k1+rfi); r[ 2]=fr(i0+j0+k2+rfi); r[ 3]=fr(i0+j0+k3+rfi);
                        r[ 4]=fr(i0+j1+k0+rfi); r[ 5]=fr(i0+j1+k1+rfi); r[ 6]=fr(i0+j1+k2+rfi); r[ 7]=fr(i0+j1+k3+rfi);
                        r[ 8]=fr(i0+j2+k0+rfi); r[ 9]=fr(i0+j2+k1+rfi); r[10]=fr(i0+j2+k2+rfi); r[11]=fr(i0+j2+k3+rfi);
                        r[12]=fr(i0+j3+k0+rfi); r[13]=fr(i0+j3+k1+rfi); r[14]=fr(i0+j3+k2+rfi); r[15]=fr(i0+j3+k3+rfi);
                        r[16]=fr(i1+j0+k0+rfi); r[17]=fr(i1+j0+k1+rfi); r[18]=fr(i1+j0+k2+rfi); r[19]=fr(i1+j0+k3+rfi);
                        r[20]=fr(i1+j1+k0+rfi); r[21]=fr(i1+j1+k1+rfi); r[22]=fr(i1+j1+k2+rfi); r[23]=fr(i1+j1+k3+rfi);
                        r[24]=fr(i1+j2+k0+rfi); r[25]=fr(i1+j2+k1+rfi); r[26]=fr(i1+j2+k2+rfi); r[27]=fr(i1+j2+k3+rfi);
                        r[28]=fr(i1+j3+k0+rfi); r[29]=fr(i1+j3+k1+rfi); r[30]=fr(i1+j3+k2+rfi); r[31]=fr(i1+j3+k3+rfi);
                        r[32]=fr(i2+j0+k0+rfi); r[33]=fr(i2+j0+k1+rfi); r[34]=fr(i2+j0+k2+rfi); r[35]=fr(i2+j0+k3+rfi);
                        r[36]=fr(i2+j1+k0+rfi); r[37]=fr(i2+j1+k1+rfi); r[38]=fr(i2+j1+k2+rfi); r[39]=fr(i2+j1+k3+rfi);
                        r[40]=fr(i2+j2+k0+rfi); r[41]=fr(i2+j2+k1+rfi); r[42]=fr(i2+j2+k2+rfi); r[43]=fr(i2+j2+k3+rfi);
                        r[44]=fr(i2+j3+k0+rfi); r[45]=fr(i2+j3+k1+rfi); r[46]=fr(i2+j3+k2+rfi); r[47]=fr(i2+j3+k3+rfi);
                        r[48]=fr(i3+j0+k0+rfi); r[49]=fr(i3+j0+k1+rfi); r[50]=fr(i3+j0+k2+rfi); r[51]=fr(i3+j0+k3+rfi);
                        r[52]=fr(i3+j1+k0+rfi); r[53]=fr(i3+j1+k1+rfi); r[54]=fr(i3+j1+k2+rfi); r[55]=fr(i3+j1+k3+rfi);
                        r[56]=fr(i3+j2+k0+rfi); r[57]=fr(i3+j2+k1+rfi); r[58]=fr(i3+j2+k2+rfi); r[59]=fr(i3+j2+k3+rfi);
                        r[60]=fr(i3+j3+k0+rfi); r[61]=fr(i3+j3+k1+rfi); r[62]=fr(i3+j3+k2+rfi); r[63]=fr(i3+j3+k3+rfi);
                    }
          
                    hf.data_[m*hf.fsize_+k*hf.width_+j] += rpers *
                    curp(curp(curp(r[ 0], r[ 1], r[ 2], r[ 3], zi),
                              curp(r[ 4], r[ 5], r[ 6], r[ 7], zi),
                              curp(r[ 8], r[ 9], r[10], r[11], zi),
                              curp(r[12], r[13], r[14], r[15], zi),
                              yi),                           
                         curp(curp(r[16], r[17], r[18], r[19], zi),
                              curp(r[20], r[21], r[22], r[23], zi),
                              curp(r[24], r[25], r[26], r[27], zi),
                              curp(r[28], r[29], r[30], r[31], zi),
                              yi),                           
                         curp(curp(r[32], r[33], r[34], r[35], zi),
                              curp(r[36], r[37], r[38], r[39], zi),
                              curp(r[40], r[41], r[42], r[43], zi),
                              curp(r[44], r[45], r[46], r[47], zi),
                              yi),                           
                         curp(curp(r[48], r[49], r[50], r[51], zi),
                              curp(r[52], r[53], r[54], r[55], zi),
                              curp(r[56], r[57], r[58], r[59], zi),
                              curp(r[60], r[61], r[62], r[63], zi),
                              yi),
                         xi);
                }
            }
        }
    }
    hf.normalize();
}

/*
class QuadFloat
{
public:
    __m128 value;
    QuadFloat(float f0, float f1, float f2, float f3);
    QuadFloat(float *);
    operator+(QuadFloat rhs)
    {
        __asm
        {
            movss xmm0, rhs
            addss
        }
    }
*/

void TextureGenerator::perlin_vn(TextureData &td, float low, float high,
                                  float lacunarity, float H,
                                  float tides, long rfi)
{
    ScalarData sd(td.width_, td.height_, td.frames_, td.fps_);
    this->perlin_vn(sd, low, high, lacunarity, H, tides, rfi);
    //for (ULONG il = 0; il < td.noe_; il++)
    //    td.data_[il] = colorMix(this->color1, this->color2, sd.data_[il]);
    td.build(sd, color1, color2);
}


void 
TextureGenerator::perlin_vn(ScalarData &hf, float low, float high,
                             float lacunarity, float H,
                             float tides, long rfi)
{
    //create a temporary ScalarData for fractal processing
    ScalarData tSc(hf.width_, hf.height_, hf.frames_, hf.fps_);

    //set aspect ratios and multipliers
    float wrcp = 1 / (float)hf.width_,
          hrcp = 1 / (float)hf.height_,
          drcp = 1 / (float)hf.frames_,
          wc, hc, dc, wcu, hcu, dcu, xi, yi, zi;
    int aratiow = hf.width_/hf.height_,
        aratioh = hf.height_/hf.width_,
        xmul, ymul, zmul;
    xmul = (aratiow > aratioh) ? ymul = 1, aratiow : ymul = aratioh, 1;

    //set frequences and iterator
    float low_freq = pow(2.0, low),
          current_freq = low_freq,
          high_freq = pow(2.0, high),
          iter_fl = log(lacunarity, high_freq/low_freq)+1;  //?ouo ia ?naeaooa? iuii int

    //band limiting
    int minimum = (hf.height_ < hf.width_) ? hf.height_ : hf.width_;
    if (minimum > pow(2.0, high-1.0f))
        high = __log2x(minimum);

    hf.clear();

    int i, j, k, m, imark = -200, jmark = -200, kmark = -200,
        i0, i1, i2, i3, it, j0, j1, j2, j3, jt, k0, k1, k2, k3, kt, k1prev = -10;
    bool rmflag = false;

    float *rval0, *rval1, *rval2, *rval3, om[64];

    for (i = 0; i < iter_fl; i++, current_freq *= lacunarity)
    {
        float intensity = pow(lacunarity, -H*i);
        int d1 = xmul*current_freq, //aa? ?eeaiui c ooniaaoei?i?coc ia ?nY?ae ia a?iae ?nio oa ?Uiu
            d2 = ymul*current_freq,
            d3 = tides*current_freq;
        long d1d2 = d1 * d2;

        rval0 = new float[d1d2];
        rval1 = new float[d1d2];
        rval2 = new float[d1d2];
        rval3 = new float[d1d2];

        for (m = 0, dcu = 0.0f; m < hf.frames_; m++, dcu += drcp, k1prev = k1)
        {
			
            dc  = dcu-(int)dcu;
            kt  = zi = d3 * dc;
            zi -= kt;
            if (kmark != kt)
            {
                kmark = kt;
                rmflag = true;
                k0  = bnd(kt-1, d3);// * d1d2;
                k1  = bnd(kt,   d3);// * d1d2;
                k2  = bnd(kt+1, d3);// * d1d2;
                k3  = bnd(kt+2, d3);// * d1d2;
            }

            if ((m == 0) && (k1prev != k1))
            {
                for (int dstep = 1; (dstep <= d1) || (dstep <= d2); dstep++)
                {
                    int rnac = (dstep-1) * (dstep-1);
                    int dstep2 = dstep * 2;
                    int dstepm1d1 = (dstep - 1) * d1;
                    long rik0prnac = irf(k0) + rnac, 
                         rik1prnac = irf(k1) + rnac, 
                         rik2prnac = irf(k2) + rnac, 
                         rik3prnac = irf(k3) + rnac;
                    for (int cch1 = 0; cch1 < dstep; cch1++)
                    {
                        int rindex = cch1*d1+(dstep-1);
                        rval0[rindex] = frf(rik0prnac + cch1);
                        rval1[rindex] = frf(rik1prnac + cch1);
                        rval2[rindex] = frf(rik2prnac + cch1);
                        rval3[rindex] = frf(rik3prnac + cch1);
                    }
                    for (int cch2 = dstep-2; cch2 >= 0; cch2--)
                    {
                        int rindex = dstepm1d1 + cch2, dstep2mcch2 = dstep2 - cch2;
                        rval0[rindex] = frf(rik0prnac + dstep2mcch2);
                        rval1[rindex] = frf(rik1prnac + dstep2mcch2);
                        rval2[rindex] = frf(rik2prnac + dstep2mcch2);
                        rval3[rindex] = frf(rik3prnac + dstep2mcch2);
                    }
                }
            }
            else if (k1prev != k1)
            {
				
                float * tfptr;
                tfptr = rval0;
                rval0 = rval1;
                rval1 = rval2;
                rval2 = rval3;
				rval3 = tfptr;
                for (int dstep = 1; (dstep < d1) || (dstep < d2); dstep++)
                {
                    int rnac = (dstep-1) * (dstep-1);
                    int dstep2 = dstep * 2;
                    int dstepm1d1 = (dstep - 1) * d1;
                    long rik3prnac = irf(k3) + rnac;
                    for (int cch1 = 0; cch1 < dstep; cch1++)
                        rval3[cch1*d1+(dstep-1)] = frf(rik3prnac + cch1);
                    for (int cch2 = dstep-2; cch2 >= 0; cch2--)
                        rval3[dstepm1d1+cch2] = frf(rik3prnac + dstep2 - cch2);
                }
            }

            for (k = 0, hcu = 0.0f; k < hf.height_; k++, hcu += hrcp)
            {
                hc  = hcu-(int)hcu;
                jt  = yi = d2 * hc;
                yi -= jt;
                if (jmark != jt)
                {
                    jmark = jt;
                    rmflag = true;
                    j0  = bnd(jt-1, d2) * d1;
                    j1  = bnd(jt,   d2) * d1;
                    j2  = bnd(jt+1, d2) * d1;
                    j3  = bnd(jt+2, d2) * d1;
                }

                for (j = 0, wcu = 0.0f; j < hf.width_ ; j++, wcu+= wrcp)
                {
                    wc  = wcu-(int)wcu;
                    it  = xi = d1 * wc;
                    xi -= it;
                    if (imark != it)
                    {
                        imark = it;
                        rmflag = true;
                        i0  = bnd(it-1, d1);
                        i1  = bnd(it,   d1);
                        i2  = bnd(it+1, d1);
                        i3  = bnd(it+2, d1);
                    }
                    
                    //if (rmflag)
                    //{
                        rmflag = false;
                        register int omidx = i0+j0;
                        om[ 0] = rval0[omidx]; om[ 1] = rval1[omidx]; om[ 2] = rval2[omidx]; om[ 3] = rval3[omidx];
                        omidx = i0+j1;
                        om[ 4] = rval0[omidx]; om[ 5] = rval1[omidx]; om[ 6] = rval2[omidx]; om[ 7] = rval3[omidx];
                        omidx = i0+j2;
                        om[ 8] = rval0[omidx]; om[ 9] = rval1[omidx]; om[10] = rval2[omidx]; om[11] = rval3[omidx];
                        omidx = i0+j3;
                        om[12] = rval0[omidx]; om[13] = rval1[omidx]; om[14] = rval2[omidx]; om[15] = rval3[omidx];
                        
                        omidx = i1+j0;
                        om[16] = rval0[omidx]; om[17] = rval1[omidx]; om[18] = rval2[omidx]; om[19] = rval3[omidx];
                        omidx = i1+j1;
                        om[20] = rval0[omidx]; om[21] = rval1[omidx]; om[22] = rval2[omidx]; om[23] = rval3[omidx];
                        omidx = i1+j2;
                        om[24] = rval0[omidx]; om[25] = rval1[omidx]; om[26] = rval2[omidx]; om[27] = rval3[omidx];
                        omidx = i1+j3;
                        om[28] = rval0[omidx]; om[29] = rval1[omidx]; om[30] = rval2[omidx]; om[31] = rval3[omidx];
                        
                        omidx = i2+j0;
                        om[32] = rval0[omidx]; om[33] = rval1[omidx]; om[34] = rval2[omidx]; om[35] = rval3[omidx];
                        omidx = i2+j1;
                        om[36] = rval0[omidx]; om[37] = rval1[omidx]; om[38] = rval2[omidx]; om[39] = rval3[omidx];
                        omidx = i2+j2;
                        om[40] = rval0[omidx]; om[41] = rval1[omidx]; om[42] = rval2[omidx]; om[43] = rval3[omidx];
                        omidx = i2+j3;
                        om[44] = rval0[omidx]; om[45] = rval1[omidx]; om[46] = rval2[omidx]; om[47] = rval3[omidx];
                        
                        omidx = i3+j0;
                        om[48] = rval0[omidx]; om[49] = rval1[omidx]; om[50] = rval2[omidx]; om[51] = rval3[omidx];
                        omidx = i3+j1;
                        om[52] = rval0[omidx]; om[53] = rval1[omidx]; om[54] = rval2[omidx]; om[55] = rval3[omidx];
                        omidx = i3+j2;
                        om[56] = rval0[omidx]; om[57] = rval1[omidx]; om[58] = rval2[omidx]; om[59] = rval3[omidx];
                        omidx = i3+j3;
                        om[60] = rval0[omidx]; om[61] = rval1[omidx]; om[62] = rval2[omidx]; om[63] = rval3[omidx];
                    //}

                    tSc.data_[m*hf.fsize_+k*hf.width_+j] = tricubic_interpolate(om, xi, yi, zi);
                    
                }
            }
        }
        delete [] rval0;
        delete [] rval1;
        delete [] rval2;
        delete [] rval3;

		tSc.normalize();
        //tSc.abs();
		for (long noecnt = 0; noecnt < hf.noe_; noecnt++)
			hf.data_[noecnt] += intensity * tSc.data_[noecnt];

    }
    hf.normalize();
}



void TextureGenerator::perlin_gn(TextureData &td, float low, float high,
                                  float lacunarity, float H,
                                  float tides, long rfi)
{
    ScalarData sd(td.width_, td.height_, td.frames_, td.fps_);
    this->perlin_gn(sd, low, high, lacunarity, H, tides, rfi);
    //for (ULONG il = 0; il < td.noe_; il++)
    //    td.data_[il] = colorMix(this->color1, this->color2, sd.data_[il]);
    td.build(sd, color1, color2);
}


void 
TextureGenerator::perlin_gn(ScalarData &hf, float low, float high,
                               float lacunarity, float H,
                               float tides, long rfi)
{
    //create a temporary ScalarData for fractal processing
    ScalarData tSc(hf.width_, hf.height_, hf.frames_, hf.fps_);

    //set aspect ratios and multipliers
    float wrcp = 1 / (float)hf.width_,
          hrcp = 1 / (float)hf.height_,
          drcp = 1 / (float)hf.frames_,
          wcu, hcu, dcu;
    int aratiow = hf.width_/hf.height_,
        aratioh = hf.height_/hf.width_,
        xmul, ymul, zmul;
    xmul = (aratiow > aratioh) ? ymul = 1, aratiow : ymul = aratioh, 1;

    //set frequences and iterator
    float low_freq = pow(2.0, low),
          current_freq = low_freq,
          high_freq = pow(2.0, high),
          iter_fl = log(lacunarity, high_freq/low_freq)+1;  //?ouo ia ?naeaooa? iuii int

    //band limiting
    int minimum = (hf.height_ < hf.width_) ? hf.height_ : hf.width_;
    if (minimum > pow(2.0, high-1.0f))
        high = __log2x(minimum);

    hf.clear();

    static float c1 = 6, c2 = -15, c3 = 10;
    float xis, xir, yis, yir, zis, zir, dxi, dyi, dzi;
    int i, j, k, m, i1, i2, j1, j2, k1, k2, rcx, rcy,
        p_k1, p_k2, p_k1j1, p_k1j2, p_k2j1, p_k2j2;

    for (i = 0; i < iter_fl; i++, current_freq *= lacunarity)
    {
        float intensity = pow(lacunarity, -H*i);
        int d1 = xmul*current_freq, //aa? ?eeaiui c ooniaaoei?i?coc ia ?nY?ae ia a?iae ?nio oa ?Uiu
            d2 = ymul*current_freq,
            d3 = tides*current_freq;
        long d1d2m4 = d1 * d2 * 4;

        for (m = 0, dcu = 0.0f; m < hf.frames_; m++, dcu += drcp)
        {
            zir = (zis -= (k1  = zis = d3 * (dcu-(int)dcu))) - 1.0f;
            __asm
            {
                movss xmm0, zis
                movss xmm1, zis
                mulss xmm0, c1
                addss xmm0, c2
                mulss xmm0, xmm1
                addss xmm0, c3
                mulss xmm0, xmm1
                mulss xmm0, xmm1
                mulss xmm0, xmm1
                movss dzi, xmm0
            }
            if ((k2 = 1 + (k1 %= d3)) == d3) k2 = 0;
            p_k1 = p_[k1];
            p_k2 = p_[k2];

            for (k = 0, hcu = 0.0f; k < hf.height_; k++, hcu += hrcp)
            {
                yir = (yis -= (j1  = yis = d2 * (hcu-(int)hcu))) - 1.0f;
                __asm
                {
                    movss xmm0, yis
                    movss xmm1, yis
                    mulss xmm0, c1
                    addss xmm0, c2
                    mulss xmm0, xmm1
                    addss xmm0, c3
                    mulss xmm0, xmm1
                    mulss xmm0, xmm1
                    mulss xmm0, xmm1
                    movss dyi, xmm0
                }
                if ((j2 = 1 + (j1 %= d2)) == d2) j2 = 0;
                p_k1j1 = p_[p_k1+j1];
                p_k1j2 = p_[p_k1+j2];
                p_k2j1 = p_[p_k2+j1];
                p_k2j2 = p_[p_k2+j2];

                for (j = 0, wcu = 0.0f; j < hf.width_ ; j++, wcu+= wrcp)
                {
                    xir = (xis -= (i1  = xis = d1 * (wcu-(int)wcu))) - 1.0f;
                    __asm
                    {
                        movss xmm0, xis
                        movss xmm1, xis
                        mulss xmm0, c1
                        addss xmm0, c2
                        mulss xmm0, xmm1
                        addss xmm0, c3
                        mulss xmm0, xmm1
                        mulss xmm0, xmm1
                        mulss xmm0, xmm1
                        movss dxi, xmm0
                    }
                    if ((i2 = 1 + (i1 %= d1)) == d1) i2 = 0;

                    tSc.data_[tSc.d_idx_[m]+tSc.h_idx_[k]+j] =
                    lerp(lerp(lerp(grds_[p_[p_k1j1+i1]] * Vector(xis, yis, zis),
                                   grds_[p_[p_k2j1+i1]] * Vector(xis, yis, zir), dzi),
                              lerp(grds_[p_[p_k1j2+i1]] * Vector(xis, yir, zis),
                                   grds_[p_[p_k2j2+i1]] * Vector(xis, yir, zir), dzi), dyi),
                         lerp(lerp(grds_[p_[p_k1j1+i2]] * Vector(xir, yis, zis),
                                   grds_[p_[p_k2j1+i2]] * Vector(xir, yis, zir), dzi),
                              lerp(grds_[p_[p_k1j2+i2]] * Vector(xir, yir, zis),
                                   grds_[p_[p_k2j2+i2]] * Vector(xir, yir, zir), dzi), dyi), dxi);
                }
            }
        }
        tSc.normalize();
        //tSc.abs();
        //tSc.bias(0.8);
        //tSc.gain(0.2);

        //tSc.bias_0_1(0.8);
        //tSc.normalize();
		for (long noecnt = 0; noecnt < hf.noe_; noecnt++)
			hf.data_[noecnt] += intensity * tSc.data_[noecnt];

    }
    hf.normalize();
}


void 
TextureGenerator::perlin_gn__(ScalarData &hf, float low, float high,
                               float lacunarity, float H,
                               float tides, long rfi)
{
    //create a temporary ScalarData for fractal processing
    ScalarData tSc(hf.width_, hf.height_, hf.frames_, hf.fps_);

    //set aspect ratios and multipliers
    float wrcp = 1 / (float)hf.width_,
          hrcp = 1 / (float)hf.height_,
          drcp = 1 / (float)hf.frames_,
          wcu, hcu, dcu;
    int aratiow = hf.width_/hf.height_,
        aratioh = hf.height_/hf.width_,
        xmul, ymul, zmul;
    xmul = (aratiow > aratioh) ? ymul = 1, aratiow : ymul = aratioh, 1;

    //set frequences and iterator
    float low_freq = pow(2.0, low),
          current_freq = low_freq,
          high_freq = pow(2.0, high),
          iter_fl = log(lacunarity, high_freq/low_freq)+1;  //?ouo ia ?naeaooa? iuii int

    //band limiting
    int minimum = (hf.height_ < hf.width_) ? hf.height_ : hf.width_;
    if (minimum > pow(2.0, high-1.0f))
        high = __log2x(minimum);

    hf.clear();

    static float c1 = 6, c2 = -15, c3 = 10;
    float xis, xir, yis, yir, zis, zir, dxi, dyi, dzi;
    int i, j, k, m, i1, i2, j1, j2, k1, k2, rcx, rcy,
        p_k1, p_k2, p_k1j1, p_k1j2, p_k2j1, p_k2j2;

    for (i = 0; i < iter_fl; i++, current_freq *= lacunarity)
    {
        float intensity = pow(lacunarity, -H*i);
        int d1 = xmul*current_freq, //aa? ?eeaiui c ooniaaoei?i?coc ia ?nY?ae ia a?iae ?nio oa ?Uiu
            d2 = ymul*current_freq,
            d3 = tides*current_freq;
        long d1d2m4 = d1 * d2 * 4;

        for (m = 0, dcu = 0.0f; m < hf.frames_; m++, dcu += drcp)
        {
            zir = (zis -= (k1  = zis = d3 * (dcu-(int)dcu))) - 1.0f;
            __asm
            {
                movss xmm0, zis
                movss xmm1, zis
                mulss xmm0, c1
                addss xmm0, c2
                mulss xmm0, xmm1
                addss xmm0, c3
                mulss xmm0, xmm1
                mulss xmm0, xmm1
                mulss xmm0, xmm1
                movss dzi, xmm0
            }
            if ((k2 = 1 + (k1 %= d3)) == d3) k2 = 0;
            p_k1 = p_[k1];
            p_k2 = p_[k2];

            for (k = 0, hcu = 0.0f; k < hf.height_; k++, hcu += hrcp)
            {
                yir = (yis -= (j1  = yis = d2 * (hcu-(int)hcu))) - 1.0f;
                __asm
                {
                    movss xmm0, yis
                    movss xmm1, yis
                    mulss xmm0, c1
                    addss xmm0, c2
                    mulss xmm0, xmm1
                    addss xmm0, c3
                    mulss xmm0, xmm1
                    mulss xmm0, xmm1
                    mulss xmm0, xmm1
                    movss dyi, xmm0
                }
                if ((j2 = 1 + (j1 %= d2)) == d2) j2 = 0;
                p_k1j1 = p_[p_k1+j1];
                p_k1j2 = p_[p_k1+j2];
                p_k2j1 = p_[p_k2+j1];
                p_k2j2 = p_[p_k2+j2];

                for (j = 0, wcu = 0.0f; j < hf.width_ ; j++, wcu+= wrcp)
                {
                    xir = (xis -= (i1  = xis = d1 * (wcu-(int)wcu))) - 1.0f;
                    __asm
                    {
                        movss xmm0, xis
                        movss xmm1, xis
                        mulss xmm0, c1
                        addss xmm0, c2
                        mulss xmm0, xmm1
                        addss xmm0, c3
                        mulss xmm0, xmm1
                        mulss xmm0, xmm1
                        mulss xmm0, xmm1
                        movss dxi, xmm0
                    }
                    if ((i2 = 1 + (i1 %= d1)) == d1) i2 = 0;

                    tSc.data_[tSc.d_idx_[m]+tSc.h_idx_[k]+j] =
                    lerp(lerp(lerp(grds_[p_[p_k1j1+i1]] * Vector(xis, yis, zis),
                                   grds_[p_[p_k2j1+i1]] * Vector(xis, yis, zir), dzi),
                              lerp(grds_[p_[p_k1j2+i1]] * Vector(xis, yir, zis),
                                   grds_[p_[p_k2j2+i1]] * Vector(xis, yir, zir), dzi), dyi),
                         lerp(lerp(grds_[p_[p_k1j1+i2]] * Vector(xir, yis, zis),
                                   grds_[p_[p_k2j1+i2]] * Vector(xir, yis, zir), dzi),
                              lerp(grds_[p_[p_k1j2+i2]] * Vector(xir, yir, zis),
                                   grds_[p_[p_k2j2+i2]] * Vector(xir, yir, zir), dzi), dyi), dxi);
                }
            }
        }
        tSc.normalize();
        tSc.abs();
        //tSc.bias(0.8);
        //tSc.gain(0.2);

        //tSc.bias_0_1(0.8);
        //tSc.normalize();
		for (long noecnt = 0; noecnt < hf.noe_; noecnt++)
			hf.data_[noecnt] += intensity * tSc.data_[noecnt];

    }
    hf.normalize();
}