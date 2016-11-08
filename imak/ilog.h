#ifndef LOG_H
#define LOG_H


inline
double __ylog2x(double y, double x)
{
   __asm
   {
      fld      y   
      fld      x   
      fyl2x        
   }
}


inline
double __log2x(double x)
{
   __asm
   {
      fld1
      fld      x
      fyl2x
   }
}


inline
double log(double b, double x)
{
   __asm
   {
      fld1
      fld      x
      fyl2x
      fld1
      fld      b
      fyl2x
      fdivp    st(1), st(0)
   }
}

#endif