#ifndef COLOR_H
#define COLOR_H

#include "imakdefs.h"


enum Layer
{
    RED,
    GREEN,
    BLUE,
    ALPHA
};


struct IColor
{
   UCHAR Red;
   UCHAR Green;
   UCHAR Blue;
   UCHAR Alpha;

   IColor() {Red = Green = Blue = Alpha = 255;}

   IColor(UCHAR r, UCHAR g, UCHAR b, UCHAR a)
   {
       Red = r; Green = g; Blue = b; Alpha = a;
   }
};

const IColor Black        ((UCHAR)0  , (UCHAR)0  , (UCHAR)0  , (UCHAR)255);
const IColor White        ((UCHAR)255, (UCHAR)255, (UCHAR)255, (UCHAR)255);
const IColor Red          ((UCHAR)255, (UCHAR)0  , (UCHAR)0  , (UCHAR)255);
const IColor Green        ((UCHAR)0  , (UCHAR)255, (UCHAR)0  , (UCHAR)255);
const IColor Blue         ((UCHAR)0  , (UCHAR)0  , (UCHAR)255, (UCHAR)255);
const IColor Yellow       ((UCHAR)255, (UCHAR)255, (UCHAR)0  , (UCHAR)255);
const IColor Cyan         ((UCHAR)255, (UCHAR)255, (UCHAR)0  , (UCHAR)255);
const IColor Magenta      ((UCHAR)255, (UCHAR)255, (UCHAR)0  , (UCHAR)255);
const IColor LightRed     ((UCHAR)255, (UCHAR)128, (UCHAR)128, (UCHAR)255);
const IColor LightGreen   ((UCHAR)128, (UCHAR)255, (UCHAR)128, (UCHAR)255);
const IColor LightBlue    ((UCHAR)128, (UCHAR)128, (UCHAR)255, (UCHAR)255);


IColor operator*(IColor & col, float fc);
IColor operator*(float fc, IColor & col);
IColor operator*(IColor & col1, IColor & col2);
IColor operator+(IColor & col1, IColor & col2);
IColor colorMix(IColor & col1, IColor & col2, float ratio);
IColor colorMax(IColor & col1, IColor & col2);


#endif
