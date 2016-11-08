#ifndef _IMAKGP_H_
#define _IMAKGP_H_

#include <cmath>

#define inline __forceinline

inline double fr(register __int32 x);
inline __int32 bnd(__int32 a, __int32 bound);
inline float curp(float v0, float v1, float v2, float v3, float x);
inline float lerp(float v1, float v2, float x);


inline float drop(float a)
{
    register float aaa = a * a * a, aaaa = aaa * a;
    return 6 * aaaa * a - 15 * aaaa + 10 * aaa;
}


inline float dropn(float a)
{
    static float c1 = 6, c2 = -15, c3 = 10;
    __asm
    {
        movss xmm2, a
        movss xmm3, xmm2
        mulss xmm2, xmm3
        movss xmm4, xmm2
        mulss xmm2, xmm3
        mulss xmm3, xmm2
        mulss xmm4, xmm2
        mulss xmm4, c1
        mulss xmm3, c2
        mulss xmm2, c3
        addss xmm2, xmm3
        addss xmm2, xmm4
        movss a, xmm2
        fld a
    }
}


inline float drop3(float x, float y, float z)
{
    return drop(x) * drop(y) * drop(z);
}


inline float rlerp(float a, float b, float t)
{
    return b + t*(a-b);
}


inline
double fr(__int32 x)
{
    x = (x<<13) ^ x;
    return (1.0 - ((x * (x * x * 15731 + 789221) + 1376312589)
                   & 0x7fffffff) / 1073741824.0);
}


inline
double frf(__int32 x)
{
    x = (x<<13) ^ x;
    return (1.0 - ((x * (x * x * 15731 + 789221) + 1376312589)
                   & 0x7fffffff) / 1073741824.0);
}



inline
long irf(__int32 x)
{
    x = (x<<13) ^ x;
    return (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff;
}



inline
__int32 bnd(__int32 a, __int32 border)
{
    __asm
    {
        mov   eax, a
        xor   edx, edx
        mov   ebx, border
        idiv  ebx
        xor   eax, eax
        cmp   edx, 0
        cmovb eax, ebx
        add   eax, edx
    }
}


inline
float curp(float v0, float v1, float v2, float v3, float x)
{
   register float p = - v0 + v1 - v2 + v3;
   register float sqrx = x * x;
   return p * sqrx * x + (v0 - v1 - p) * sqrx + (v2 - v0) * x + v1;
}

//inline
//float tri2i(float *data, float tx, float ty, float tz)
//{
//    float t1[4], t2[2], t3;
//    t1[0] = data[0] + tz * (data[4] - data[0]);
//    t1[1] = data[1] + tz * (data[5] - data[1]);
//    t1[2] = data[2] + tz * (data[6] - data[2]);
//    t1[3] = data[3] + tz * (data[7] - data[3]);
//    t2[0] = t1[0] + ty * (t1[2] - t1[0]);
//    t2[1] = t1[1] + ty * (t1[3] - t1[1]);
//    t3 =  t2[0] + tx * (t2[1] - t2[0]);
//    float du, dv, dw;
//}

inline
float tricurve_interpolate(float *data, float tx, float ty, float tz)
{
    float a = 6.0f, b = -15.0f, c = 10.0f, tmp[4], *tp;
    tp = tmp;
    __asm
    {
        mov eax, data
        movups xmm0, [eax]
        add eax, 16
        movups xmm1, [eax]

        movss xmm2, tz
        movss xmm3, xmm2
        mulss xmm2, xmm3
        movss xmm4, xmm2
        mulss xmm2, xmm3
        mulss xmm3, xmm2
        mulss xmm4, xmm2
        mulss xmm4, a
        mulss xmm3, b
        mulss xmm2, c
        addss xmm2, xmm3
        addss xmm2, xmm4

        shufps xmm2, xmm2, 0

        subps xmm1, xmm0
        mulps xmm1, xmm2
        addps xmm1, xmm0

        movaps xmm0, xmm1  //ayto prepei na fygei

        movhlps xmm1, xmm0

        movss xmm2, ty
        movss xmm3, xmm2
        mulss xmm2, xmm3
        movss xmm4, xmm2
        mulss xmm2, xmm3
        mulss xmm3, xmm2
        mulss xmm4, xmm2
        mulss xmm4, a
        mulss xmm3, b
        mulss xmm2, c
        addss xmm2, xmm3
        addss xmm2, xmm4

        shufps xmm2, xmm2, 0

        subps xmm1, xmm0
        mulps xmm1, xmm2
        addps xmm1, xmm0

        movaps xmm0, xmm1 //kai ayto na fygei

        shufps xmm1, xmm0, 0x55
        movhlps xmm1, xmm1

        movss xmm2, tx
        movss xmm3, xmm2
        mulss xmm2, xmm3
        movss xmm4, xmm2
        mulss xmm2, xmm3
        mulss xmm3, xmm2
        mulss xmm4, xmm2
        mulss xmm4, a
        mulss xmm3, b
        mulss xmm2, c
        addss xmm2, xmm3
        addss xmm2, xmm4

        subss xmm1, xmm0
        mulss xmm1, xmm2
        addss xmm1, xmm0

        movaps xmm0, xmm1  //kai ayto epishs

        movss tx, xmm0
        fld tx
    }
}



inline
float tricubic_interpolate(float *data, float tx, float ty, float tz)
{
    __asm
    {
        mov eax, data
        mov ebx, eax
        add ebx, 16
        mov ecx, ebx
        add ecx, 16
        mov edx, ecx
        add edx, 16
        xor esi, esi

        movss xmm5, ty

loopa:
        inc esi

        movups xmm0, [eax]
        movups xmm1, [ebx]
        movups xmm2, [ecx]
        movups xmm3, [edx]

        movaps xmm4, xmm1
        addps xmm4, xmm3
        subps xmm4, xmm0
        subps xmm4, xmm2

        unpcklps xmm5, xmm5
        movlhps xmm5, xmm5

        movaps xmm3, xmm5
        mulps xmm3, xmm3

        movaps xmm6, xmm0
        subps xmm6, xmm1
        subps xmm6, xmm4
        mulps xmm6, xmm3

        movaps xmm7, xmm2
        subps xmm7, xmm0
        mulps xmm7, xmm5
        addps xmm7, xmm1

        addps xmm6, xmm7

        mulps xmm4, xmm3
        mulps xmm4, xmm5

        addps xmm6, xmm4
        movups [eax], xmm6

        cmp esi, 4
        jae step2
        add eax, 64
        add ebx, 64
        add ecx, 64
        add edx, 64
        jmp loopa
step2:
        cmp esi, 5
        jae step3
        mov eax, data
        mov ebx, eax
        add ebx, 64
        mov ecx, ebx
        add ecx, 64
        mov edx, ecx
        add edx, 64
        movss xmm5, tx
        jmp loopa
step3:
        cmp esi, 6
        jae end
        mov ebx, eax
        add ebx, 4
        mov ecx, ebx
        add ecx, 4
        mov edx, ecx
        add edx, 4
        movss xmm5, tz
        jmp loopa
end:
        movss tx, xmm6
        fld tx
    }
}



inline
float lerp(float v1, float v2, float x)
{
   return v1 + x * (v2 - v1);
}

#endif