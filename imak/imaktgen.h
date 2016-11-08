#ifndef _IMAKTEXGEN_H_
#define _IMAKTEXGEN_H_


//#include "imakdefs.h"
//#include "color.h"


#include <memory.h>
#include <stdlib.h>
#include "imakdefs.h"
#include "icolor.h"
#include "3dmath.h"


class TextureData;
class ScalarData;


class TextureData
{
public:
    UCHAR wexp_;
    UCHAR hexp_;
    UINT width_;
    UINT height_;
    UINT fsize_;
    ULONG noe_;
    float fps_;
    USHORT frames_;
public:
	IColor * data_;

    TextureData(): data_(NULL), noe_(0){};
    TextureData(UCHAR wexp, UCHAR hexp, USHORT frames = 1, float fps = 0):
        data_(NULL), noe_(0)
    {
        build(wexp, hexp, frames, fps);
    }
    ~TextureData()
    {
        this->demolish();
    }
    void build(UCHAR, UCHAR, USHORT = 1, float = 0);
    void build(ScalarData &, IColor, IColor);
    void demolish();
    void envmap();
    void invert();
	void setAlphaAvg();
	void postMask(IColor);
    void setlayer(Layer, UCHAR);
    friend class TextureGenerator;
    friend class ScalarData;
};


class ScalarData
{
public:
    UINT width_;
    UINT height_;
    UINT fsize_;
    ULONG noe_;
    float fps_;
    USHORT frames_;
    float * data_;
    int * h_idx_, * d_idx_;
//public:
    ScalarData(): data_(NULL), noe_(0){};
    ScalarData(UINT width, UINT height, USHORT frames = 1, float fps = 0):
        data_(NULL), noe_(0)
    {
        build(width, height, frames, fps);
    }
    ~ScalarData()
    {
        this->demolish();
    }
    void build(UINT, UINT, USHORT = 1, float = 0);
    void demolish();
    void clear();
    void normalize();
    void pntransform();
    void ptransform();
    void mult(float);
    void raise(float);
    void abs();
    void bias(float);
    void gain(float);
    void invert();
    void map(ScalarData &hf);
    void slaughter(float, float);
    void shift(ScalarData &hf, float, float, float);
    friend class TextureData;
    friend class TextureGenerator;
    friend class Mountains;
    friend class Water;
};


#define ETN 8192

class TextureGenerator
{
private:
    void noise(ScalarData &hf, float octave, USHORT rfi);
    USHORT p_[2 * ETN];
    Vector grds_[ETN];
public:
    TextureGenerator(): color1(Black), color2(White)
    {
        Vector vec;
        for (int i=0; i < ETN; i++)
        {
            p_[i] = rand() % ETN;
            vec.x = (float)(rand() - RAND_MAX/2)/RAND_MAX;
            vec.y = (float)(rand() - RAND_MAX/2)/RAND_MAX;
            vec.z = (float)(rand() - RAND_MAX/2)/RAND_MAX;
            grds_[i] = vec / vec.Magnitude();
        }
        memcpy(p_+ETN, p_, ETN);
    };
    ~TextureGenerator(){};
    IColor color1;
    IColor color2;
    void perlin(ScalarData &td, UCHAR low = 3, UCHAR high = 8,
                UCHAR lacunarity = 3, UCHAR persistence = 8, UCHAR rfi = 0,
                UCHAR tides = 1);
    void perlin(TextureData &td, UCHAR low = 3, UCHAR high = 8,
                UCHAR lacunarity = 3, UCHAR persistence = 8, UCHAR rfi = 0,
                UCHAR tides = 1);
    void perlin_vn(ScalarData &hf, float low = 3.0f, float high = 8.0f,
                   float lacunarity = 2.0f, float H = 0.5f,
                   float tides = 1.0f, long rfi = 0);
    void perlin_vn(TextureData &td, float low = 3.0f, float high = 8.0f,
                   float lacunarity = 2.0f, float H = 0.5f,
                   float tides = 1.0f, long rfi = 0);
    void perlin2(ScalarData &hf, float low = 3.0f, float high = 8.0f,
                 float lacunarity = 2.0f, float H = 0.5f,
                 float tides = 1.0f, long rfi = 0, float aratio = 0.0f);
    void perlin_gn(ScalarData &td, float low = 3.0f, float high = 8.0f,
                   float lacunarity = 2.0f, float H = 0.5f,
                   float tides = 1.0f, long rfi = 0);
    void perlin_gn__(ScalarData &td, float low = 3.0f, float high = 8.0f,
                     float lacunarity = 2.0f, float H = 0.5f,
                     float tides = 1.0f, long rfi = 0);
    void perlin_gn(TextureData &td, float low = 3.0f, float high = 8.0f,
                   float lacunarity = 2.0f, float H = 0.5f,
                   float tides = 1.0f, long rfi = 0);
};
#endif