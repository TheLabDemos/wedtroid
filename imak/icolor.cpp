
#include "icolor.h"

IColor operator*(IColor & col, float fc)
{
   IColor rc;
   rc.Red   = col.Red   * fc,
   rc.Green = col.Green * fc,
   rc.Blue  = col.Blue  * fc,
   rc.Alpha = col.Alpha * fc;
   return rc;
}


IColor operator*(float fc, IColor & col)
{
   return col * fc;
}

IColor operator*(IColor & col1, IColor & col2)
{
    IColor rc;
    rc.Red   = col1.Red   * col2.Red   /255,
    rc.Green = col1.Green * col2.Green /255,
    rc.Blue  = col1.Blue  * col2.Blue  /255,
    rc.Alpha = col1.Alpha * col2.Alpha /255;
    return rc;
}


IColor operator+(IColor & col1, IColor & col2)
{
   short nRed   = col1.Red   + col2.Red  ;
   short nBlue  = col1.Green + col2.Green;
   short nGreen = col1.Blue  + col2.Blue ;
   short nAlpha = col1.Alpha + col2.Alpha;
   IColor rc;
   rc.Red   = (nRed   < 256) ? nRed   : 255 ;
   rc.Green = (nGreen < 256) ? nGreen : 255 ;
   rc.Blue  = (nBlue  < 256) ? nBlue  : 255 ;
   rc.Alpha = (nAlpha < 256) ? nAlpha : 255 ;
   return rc;
}


IColor colorMix(IColor & col1, IColor & col2, float ratio)
{
   IColor rc; 
   rc.Red   = ratio * (col1.Red   - col2.Red  ) + col2.Red  ;
   rc.Green = ratio * (col1.Green - col2.Green) + col2.Green;
   rc.Blue  = ratio * (col1.Blue  - col2.Blue ) + col2.Blue ;
   rc.Alpha = ratio * (col1.Alpha - col2.Alpha) + col2.Alpha;
   return rc;
}


IColor colorMax(IColor & col1, IColor & col2)
{
   IColor rc;
   rc.Red   = (col1.Red   > col2.Red)   ? col1.Red   : col2.Red;
   rc.Green = (col1.Green > col2.Green) ? col1.Green : col2.Green;
   rc.Blue  = (col1.Blue  > col2.Blue)  ? col1.Blue  : col2.Blue;
   rc.Alpha = (col1.Alpha > col2.Alpha) ? col1.Alpha : col2.Alpha;
   return rc;
}