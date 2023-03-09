//-< CCALC.CPP >-----------------------------------------------------*--------*
// Ccalc                      Version 1.02       (c) 1998  GARRET    *     ?  *
// (C expression command line calculator)                            *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Oct-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 20-Oct-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
/*
Input for this calculator are normal C expressions containing operators, float
or integer constants, variables and and references to previous results (@n).
Precedence and semantic of operators is the same as in C language. There are
two extra binary operators: >>> unsigned shift right and ** raising to power.
Calculator supports standard set of functions from C mathematics library and
also defines function prime(n) for the smallest prime number >= n, gcd(n,m) for
the greatest common divisor of n and m, and invmod(n,m) for inverse of n modulo m.
Operators:
        ++ -- ! ~ unary + -
        **
	* / % //
        + -
        << >> >>>
        < <= > >=
        = == != <>
        &
        ^ (power in PAS style)
        # (XOR)
        |
        := = += -= *= /= %= <<= >>= >>>= &= |= #= ^= **=
Functions:
        abs, atan, cosh, float, prime, sqrt,
        acos, atan2, exp, log, sin, tan,
        asin, cos, int, log10, sinh, tanh,
        gcd, invmod, arccos, arctg, sing, cosg,
        tg, tgg, ctg, ctgg, exp10, frac, ln, lg,
        round, not, min, max

Constants:
        pi, e.
*/

#if defined(_WIN64)||defined(_WIN32)
#define _WIN_
#define INT_FORMAT      "L"
#include <windows.h>
#endif

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <errno.h>


#pragma hdrstop
#include "scalc.h"
#include "sfmts.h"
#pragma package(smart_init)

#pragma warn -8027
#pragma warn -8004
#pragma warn -8080
#pragma warn -8071

static int_t To_int(int_t val)
{
 return val;
}

static float_t To_float(float_t val)
{
 return val;
}

static int_t Prime(int_t n)
{
 if (n <= 3) return n;
 n |= 1;
 while (true)
  {
   int_t m = (int_t)sqrt((float_t)n) + 1;
   int_t k = 3;
   while (true)
    {
     if (k > m) return n;
     if (n % k == 0) break;
     k += 2;
    }
   n += 2;
  }
}

static char* to_bin(int nval, char* pbuf, int nbufsize)
{
  int ncnt;
  int bcnt;
  int nlen = sizeof(int) * 8 + sizeof(int);

  if (pbuf != NULL && nbufsize > nlen)
   {
    pbuf[nlen] = '\0';
    pbuf[nlen - 1] = 'b';
    for (ncnt = 0, bcnt = nlen - 2; ncnt < nlen; ncnt ++, bcnt --)
     {
      if (ncnt > 0 && (ncnt % 8) == 0)
       {
        pbuf[bcnt] = '.';
        bcnt --;
       }
      pbuf[bcnt] = (nval & (1 << ncnt))? '1' : '0';
     }
   }
  return pbuf;
}

static int_t Gcd(int_t x, int_t y)
{
 while (x)
  {
   int_t r = y%x;
   y=x;
   x=r;
  }
 return y;
}

static int_t Invmod(int_t x, int_t y)
{
 int_t m = y;
 int_t u = 1, v = 0;
 int_t s = 0, t = 1;
 while (x)
  {
   int_t q = y/x;
   int_t r = y%x;
   int_t a = s - q*u;
   int_t b = t - q*v;
   y=x;
   s=u;
   t=v;
   x=r;
   u=a;
   v=b;
  }
 if (y!=1) return 0;
 while (s<0) s+=m;
 return s;
}

static int_t Not(int_t n)
{
 return n^-1L;
}

static int_t Now(int_t n)
{
  //time_t rawtime;
  //struct tm * timeinfo;
  //time(&rawtime);
  //timeinfo = localtime(&rawtime);
  //timeinfo = gmtime(&rawtime);
  //return (int_t)mktime(timeinfo);
  return (int_t)time(NULL)+n*60*60;
}

float_t Erf(float_t x)
{
 // constants
 float_t a1 =  0.254829592;
 float_t a2 = -0.284496736;
 float_t a3 =  1.421413741;
 float_t a4 = -1.453152027;
 float_t a5 =  1.061405429;
 float_t p  =  0.3275911;

 // Save the sign of x
 int sign = (x < 0)?-1:1;
 x = fabsl(x);

 // A&S formula 7.1.26
 float_t t = 1.0/(1.0 + p*x);
 float_t y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*expl(-x*x);

 return sign*y;
}
/*
//https://vak.dreamwidth.org/993299.html
int_t nearly_equal(double a, double b, int ignore_nbits)
{
    if (a == b)
        return true;

    // Разбиваем числа на мантиссу и экспоненту.
    int a_exponent, b_exponent;
    double a_mantissa = frexp(a, &a_exponent);
    double b_mantissa = frexp(b, &b_exponent);

    // Экспоненты обязаны совпасть.
    if (a_exponent != b_exponent)
        return false;

    // Вычитаем мантиссы, образуем положительную дельту.
    double delta = fabs(a_mantissa - b_mantissa);

    // Определяем порог допустимой разницы.
    double limit = ldexp(1.0, ignore_nbits - 52);

    return delta < limit?1:0;
}
*/
float_t Erfc(float_t x)
{
 return 1-Erf(x);
}

float_t Random(float_t x)
{
 return rand()*x/RAND_MAX;
}

static float_t Atan(float_t x)
{
 #ifdef _long_double_
 return atanl(x);
 #else
 return atan(x);
 #endif
}

static float_t Atan2l(float_t x, float_t y)
{
 if (x != 0 && y != 0)
  {
   #ifdef _long_double_
   return atan2l(x, y);
   #else
   return atan2(x, y);
   #endif
  }
 else return 0.0/0.0;
}

static float_t Cos(float_t x)
{
 #ifdef _long_double_
 return cosl(x);
 #else
 return cos(x);
 #endif
}

static float_t Cosh(float_t x)
{
 #ifdef _long_double_
 return coshl(x);
 #else
 return cosh(x);
 #endif
}

static float_t Exp(float_t x)
{
 #ifdef _long_double_
 return expl(x);
 #else
 return exp(x);
 #endif
}

static float_t Sin(float_t x)
{
 #ifdef _long_double_
 return sinl(x);
 #else
 return sin(x);
 #endif
}

static float_t Sinh(float_t x)
{
 #ifdef _long_double_
 return sinhl(x);
 #else
 return sinh(x);
 #endif
}

static float_t Tan(float_t x)
{
 #ifdef _long_double_
 return tanl(x);
 #else
 return tan(x);
 #endif
}

static float_t Tanh(float_t x)
{
 #ifdef _long_double_
 return tanhl(x);
 #else
 return tanh(x);
 #endif
}

static float_t Pow(float_t x, float_t y)
{
 if ((x > 0) || ((x == 0) && (y > 0)) ||
     ((x < 0) && (y - floor(y)==0)))
  {
   #ifdef _long_double_
   return powl(x, y);  //x^y
   #else
   return pow(x, y);
   #endif
  }
 else return 0.0/0.0;
}

static float_t Sqrt(float_t x)
{
 //return sqrtl(x);
 #ifdef _long_double_
 if (x>=0) return sqrtl(x);
 else return 0.0/0.0;
 #else
 if (x>=0) return sqrt(x);
 else return 0.0/0.0;
 #endif
}

float_t Hypot(float_t x, float_t y)
{
 return Sqrt(x*x+y*y);
}

static float_t Acos(float_t x)
{
 #ifdef _long_double_
 if ((x<=1)&&(x>=-1)) return acosl(x);
 else return 0.0/0.0;
 #else
 if ((x<=1)&&(x>=-1)) return acos(x);
 else return 0.0/0.0;
 #endif
}

static float_t Asin(float_t x)
{
 #ifdef _long_double_
 if ((x<=1)&&(x>=-1)) return asinl(x);
 else return 0.0/0.0;
 #else
 if ((x<=1)&&(x>=-1)) return asin(x);
 else return 0.0/0.0;
 #endif
}

static float_t Log(float_t x)
{
 #ifdef _long_double_
 if (x>0) return logl(x);
 else return 0.0/0.0;
 #else
 if (x>0) return log(x);
 else return 0.0/0.0;
 #endif
}

static float_t Lg(float_t x)
{
 #ifdef _long_double_
 if (x>0) return log10l(x);
 else return 0.0/0.0;
 #else
 if (x>0) return log10(x);
 else return 0.0/0.0;
 #endif
}

static float_t NP(float_t x)
{
 #ifdef _long_double_
 if (x>0) return 20*log10l(x);
 else return 0.0/0.0;
 #else
 if (x>0) return 20*log10(x);
 else return 0.0/0.0;
 #endif
}

static float_t DB(float_t x)
{
 #ifdef _long_double_
 if (x>0) return 10*log10l(x);
 else return 0.0/0.0;
 #else
 if (x>0) return 10*log10(x);
 else return 0.0/0.0;
 #endif
}

static float_t Ctanh(float_t x)  //cth
{
 #ifdef _long_double_
 if (x>0) return (expl(2*x)+1)/(expl(2*x)-1);
 else return 0.0/0.0;
 #else /*_long_double_*/
 if (x>0) return (exp(2*x)+1)/(exp(2*x)-1);
 else return 0.0/0.0;
 #endif /*_long_double_*/
}

static float_t Arsh(float_t x)
{
 #ifdef _long_double_
 return logl(x+sqrt(x*x+1));
 #else /*_long_double_*/
 return log(x+sqrt(x*x+1));
 #endif /*_long_double_*/
}

static float_t Arch(float_t x)
{
 #ifdef _long_double_
 return logl(x+sqrtl(x*x-1));
 #else /*_long_double_*/
 return log(x+sqrt(x*x-1));
 #endif /*_long_double_*/
}

static float_t Arth(float_t x)
{
 if ((x==1)||(x==-1)) return 0.0/0.0;
 #ifdef _long_double_
 return logl((1+x)/(1-x))/2;
 #else /*_long_double_*/
 return log((1+x)/(1-x))/2;
 #endif /*_long_double_*/
}

static float_t Arcth(float_t x)
{
 if ((x==1)||(x==-1)) return 0.0/0.0;
 #ifdef _long_double_
 return logl((x+1)/(x-1))/2;
 #else /*_long_double_*/
 return log((x+1)/(x-1))/2;
 #endif /*_long_double_*/
}

static float_t Exp10(float_t x)
{
 #ifdef _long_double_
 return expl(x*logl(10.0));
 #else /*_long_double_*/
 return exp(x*log(10.0));
 #endif /*_long_double_*/
}

static float_t ANP(float_t x)
{
 return Exp10(x/20);
}

static float_t ADB(float_t x)
{
 return Exp10(x/10);
}

static float_t Sing(float_t x)
{
 #ifdef _long_double_
 return sinl(M_PI*x/180);
 #else /*_long_double_*/
 return sin(M_PI*x/180);
 #endif /*_long_double_*/
}

static float_t Cosg(float_t x)
{
 #ifdef _long_double_
 return cosl(M_PI*x/180);
 #else /*_long_double_*/
 return cos(M_PI*x/180);
 #endif /*_long_double_*/
}

static float_t Tgg(float_t x)
{
 #ifdef _long_double_
 return tanl(M_PI*x/180);
 #else /*_long_double_*/
 return tan(M_PI*x/180);
 #endif /*_long_double_*/
}

static float_t Ctgg(float_t x)
{
 if (x==0) return 1/0.0;
 #ifdef _long_double_
 return 1/tanl(M_PI*x/180);
 #else /*_long_double_*/
 return 1/tan(M_PI*x/180);
 #endif /*_long_double_*/
}

static float_t Ctg(float_t x)
{
 if (x==0) return 1/0.0;
 #ifdef _long_double_
 return 1/tanl(x);
 #else /*_long_double_*/
 return 1/tan(x);
 #endif /*_long_double_*/
}

static float_t Round(float_t x)
{
 return (float_t)((int)(x+0.5));
}

static float_t Frac(float_t x)
{
 float_t d;
 #ifdef _long_double_
 return modfl(x, &d);
 #else
 return modf(x, &d);
 #endif
}

static float_t Min(float_t x, float_t y)
{
 return (x < y)? x:y;
}

static float_t Log2(float_t x)
{
 if (x <=0) return 0.0/0.0;
 #ifdef _long_double_
 return logl(x)/logl(2.0);
 #else /*_long_double_*/
 return log(x)/log(2.0);
 #endif /*_long_double_*/
}

/*
static float_t powl(float_t x, float_t y)
{
  return exp(y*log(x));
}
*/
static float_t Logn(float_t x, float_t y)
{
 if (x <=0) return 0.0/0.0;
 if (y <=0) return 0.0/0.0;
 #ifdef _long_double_
 return logl(y)/logl(x);
 #else /*_long_double_*/
 return log(y)/log(x);
 #endif /*_long_double_*/
}

static float_t Root3(float_t x)
{
 #ifdef _long_double_
 return powl(x, 1/3.0);
 #else /*_long_double_*/
 return pow(x, 1/3.0);
 #endif /*_long_double_*/
}

static float_t Rootn(float_t x, float_t y)
{
 #ifdef _long_double_
 return powl(x, 1/y);
 #else /*_long_double_*/
 return pow(x, 1/y);
 #endif /*_long_double_*/
}

/*SWG Table https://en.wikipedia.org/wiki/Standard_wire_gauge*/
double tswg[]=
{
 12.700, 11.786, 10.973, 10.160, 9.449, 8.839, 8.230, 7.620, 7.010, 6.401,
 5.893, 5.385, 4.877, 4.470, 4.064, 3.658, 3.251, 2.946, 2.642, 2.337,
 2.032, 1.829, 1.626, 1.422, 1.219, 1.016, 0.914, 0.813, 0.711, 0.610,
 0.559, 0.5080, 0.4572, 0.4166, 0.3759, 0.3454, 0.3150, 0.2946, 0.2743,
 0.2540, 0.2337, 0.2134, 0.1930, 0.1727, 0.1524, 0.1321, 0.1219, 0.1118,
 0.1016, 0.0914, 0.0813, 0.0711, 0.0610, 0.0508, 0.0406, 0.0305, 0.0254
};

float_t Swg(float_t x) //SWG to mm
{
 int n = x;
 if ((n >= -6) && (n <= 50)) return tswg[n+6];
 else return 0;
}

float_t SSwg(float_t x) //SWG to mm^2
{
 return pow(Swg(x), 2)*M_PI/4.0;
}


float_t Aswg(float_t x) //mm to SWG
{
 if ((x < 0.0254) || (x > 12.7)) return 0.0/0.0;
 int n = 0;
 while (tswg[n] >= x) n++;
 if (n)
  {
   if (fabs(tswg[n]-x) < fabs(tswg[n-1]-x)) return n-6;
   else return n-7;
  }
 else return -6;
}

/* https://en.wikipedia.org/wiki/American_wire_gauge */
static float_t Awg(float_t x) //AWG to mm
{
 return exp(2.1104-0.11594*x);
}

static float_t SAwg(float_t x) //AWG to mm^2
{
 return pow(exp(2.1104-0.11594*x), 2)*M_PI/4.0;
}

static float_t Cs(float_t x) //diameter to mm^2
{
 return pow(x, 2)*M_PI/4.0;
}

static float_t Acs(float_t x) //mm^2 to diameter
{
 return sqrt(4.0*x/M_PI);
}

static float_t Aawg(float_t x) //mm to AWG
{
 return (2.1104-log(x))/0.11594;
}

/* https://en.wikipedia.org/wiki/E_series_of_preferred_numbers */
double E3[] =   {1.0, 2.2, 4.7};
double E6[] =   {1.0, 1.5, 2.2, 3.3, 4.7, 6.8};
double E12[] =  {1.0, 1.2, 1.5, 1.8, 2.2, 2.7, 3.3, 3.9, 4.7, 5.6, 6.8, 8.2};
double E24[] =  {1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 2.7, 3.0,
                 3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2};
double E48[] =  {1.00, 1.05, 1.10, 1.15, 1.21, 1.27, 1.33, 1.40, 1.47, 1.54,
                 1.62, 1.69, 1.78, 1.87, 1.96, 2.05, 2.15, 2.26, 2.37, 2.49,
                 2.61, 2.74, 2.87, 3.01, 3.16, 3.32, 3.48, 3.65, 3.83, 4.02,
                 4.22, 4.42, 4.64, 4.87, 5.11, 5.36, 5.62, 5.90, 6.19, 6.49,
                 6.81, 7.15, 7.50, 7.87, 8.25, 8.66, 9.09, 9.53};
double E96[] =  {1.00, 1.02, 1.05, 1.07, 1.10, 1.13, 1.15, 1.18, 1.21, 1.24,
                 1.27, 1.30, 1.33, 1.37, 1.40, 1.43, 1.47, 1.50, 1.54, 1.58,
                 1.62, 1.65, 1.69, 1.74, 1.78, 1.82, 1.87, 1.91, 1.96, 2.00,
                 2.05, 2.10, 2.15, 2.21, 2.26, 2.32, 2.37, 2.43, 2.49, 2.55,
                 2.61, 2.67, 2.74, 2.80, 2.87, 2.94, 3.01, 3.09, 3.16, 3.24,
                 3.32, 3.40, 3.48, 3.57, 3.65, 3.74, 3.83, 3.92, 4.02, 4.12,
                 4.22, 4.32, 4.42, 4.53, 4.64, 4.75, 4.87, 4.99, 5.11, 5.23,
                 5.36, 5.49, 5.62, 5.76, 5.90, 6.04, 6.19, 6.34, 6.49, 6.65,
                 6.81, 6.98, 7.15, 7.32, 7.50, 7.68, 7.87, 8.06, 8.25, 8.45,
                 8.66, 8.87, 9.09, 9.31, 9.53, 9.76};
double E192[] = {1.00, 1.01, 1.02, 1.04, 1.05, 1.06, 1.07, 1.09, 1.10, 1.11,
                 1.13, 1.14, 1.15, 1.17, 1.18, 1.20, 1.21, 1.23, 1.24, 1.26,
                 1.27, 1.29, 1.30, 1.32, 1.33, 1.35, 1.37, 1.38, 1.40, 1.42,
                 1.43, 1.45, 1.47, 1.49, 1.50, 1.52, 1.54, 1.56, 1.58, 1.60,
                 1.62, 1.64, 1.65, 1.67, 1.69, 1.72, 1.74, 1.76, 1.78, 1.80,
                 1.82, 1.84, 1.87, 1.89, 1.91, 1.93, 1.96, 1.98, 2.00, 2.03,
                 2.05, 2.08, 2.10, 2.13, 2.15, 2.18, 2.21, 2.23, 2.26, 2.29,
                 2.32, 2.34, 2.37, 2.40, 2.43, 2.46, 2.49, 2.52, 2.55, 2.58,
                 2.61, 2.64, 2.67, 2.71, 2.74, 2.77, 2.80, 2.84, 2.87, 2.91,
                 2.94, 2.98, 3.01, 3.05, 3.09, 3.12, 3.16, 3.20, 3.24, 3.28,
                 3.32, 3.36, 3.40, 3.44, 3.48, 3.52, 3.57, 3.61, 3.65, 3.70,
                 3.74, 3.79, 3.83, 3.88, 3.92, 3.97, 4.02, 4.07, 4.12, 4.17,
                 4.22, 4.27, 4.32, 4.37, 4.42, 4.48, 4.53, 4.59, 4.64, 4.70,
                 4.75, 4.81, 4.87, 4.93, 4.99, 5.05, 5.11, 5.17, 5.23, 5.30,
                 5.36, 5.42, 5.49, 5.56, 5.62, 5.69, 5.76, 5.83, 5.90, 5.97,
                 6.04, 6.12, 6.19, 6.26, 6.34, 6.42, 6.49, 6.57, 6.65, 6.73,
                 6.81, 6.90, 6.98, 7.06, 7.15, 7.23, 7.32, 7.41, 7.50, 7.59,
                 7.68, 7.77, 7.87, 7.96, 8.06, 8.16, 8.25, 8.35, 8.45, 8.56,
                 8.66, 8.76, 8.87, 8.98, 9.09, 9.20, 9.31, 9.42, 9.53, 9.65,
                 9.76, 9.88};
static float_t Ee(float_t x, float_t y) //find standard value
{
 if (y)
  {
   int n;
   int N = x;
   double delta_n = 10;
   double delta_n1;
   double V = fabs(y);

   /* normalize */
   int ex= 0;
   while (V >= 1) {V /= 10; ex++;}
   while (V < 1)  {V *= 10; ex--;}

   if (N <= 192)
    {
     int nn = log10(V)*N+0.5;
     switch(N)
      {
       case 3:
        V = E3[nn];
       break;
       case 6:
        V = E6[nn];
       break;
       case 12:
        V = E12[nn];
       break;
       case 24:
        V = E24[nn];
       break;
       case 48:
        V = E48[nn];
       break;
       case 96:
        V = E96[nn];
       break;
       case 192:
        V = E192[nn];
       break;
       default:
        V = 0.0/0.0;
       break;
      }
    }
   else
    {
     for(n = 0; n < N; n++)
      {
       double v = exp(log(10.0)*n/N);
       delta_n1 = fabs(V-v);
       if (delta_n1 > delta_n)
        {
         break;
        }
       delta_n = delta_n1;
      }
     V = (floor((0.5+exp(log(10.0)*(n)/N)*100)))/100;
    }
   /* denormalize */
   while(ex)
    if (ex > 0) {V *= 10.0; ex--;}
    else {V /= 10.0; ex++;}

   if (y < 0) return -V;
   else return V;
 }
 else return 0;
}

static float_t Max(float_t x, float_t y)
{
 return (x > y)? x:y;
}

static float_t Factorial(float_t x)
{
 int n = (int)x;
 float_t res = 1.0;
 for(; n > 1; n--) res *= n;
 return res;
}

//Vref=Vout*Rl/(Rh+Rl)
//Vout=Vref*(Rh+Rl)/Rl
static float_t Vout(float_t Vref, float_t Rh, float_t Rl)
{
 return Vref*(Rh+Rl)/Rl;
}


static int _matherr(struct _exception *e)
{
  return 0;             /* error has been handled */
}

static int _matherrl(struct _exception *e)
{
  return 0;             /* error has been handled */
}


bool IsNaN(const double fVal)
{
 return
  (((*(__int64*)(&fVal) & 0x7FF0000000000000i64) == 0x7FF0000000000000i64) &&
   ((*(__int64*)(&fVal) & 0x000FFFFFFFFFFFFFi64) != 0x0000000000000000i64));
}


bool IsNaNL(const long double ldVal)
{
 return IsNaN((double)ldVal);
}

template <class T> T tmax(T x, T y)
{
 return (x > y) ? x : y;
}

template <class T> bool tisnan(T f)
{
 T _nan =  (T)0.0/(T)0.0;
 return 0 == memcmp( (void*)&f, (void*)&_nan, sizeof(T) );
}

/*
bool IsNaNL(const long float_t ldVal)
{
 typedef union
 {
  long float_t ld;
  short w[5];
  struct Parts
   {
    __int64 frac;
    short exp;
   }parts;
 } *pextrec;

 short e = (*(pextrec)&ldVal).parts.exp;
 __int64 f = (*(pextrec)&ldVal).parts.frac;
 short ww[5];
 for(int i=0; i<5; i++) ww[i] = (*(pextrec)&ldVal).w[i];
 return IsNaN((float_t)ldVal);
 return (((*(pextrec)&ldVal).parts.exp & 0x7fff == 0x7fff) &&
         ((*(pextrec)&ldVal).parts.frac & 0x7FFFFFFFFFFFFFFFi64 != 0));
}
*/


static int fmtc(char *dst, char *fmt)
{
 char c;
 int i=0;
 do
  {
   c = fmt[i];
   if (c == '%') break;
   dst[i++] = c;
  }
 while (c && (i < STRBUF));
 dst[i] = '\0';
 return i;
}

//prn("%1.30LG",(x:=1.84467440737095536e19;(x-1)/x))

static int_t fprn(char *dest, char *sfmt, int args, value* v_stack)
{
 char pfmt[STRBUF];
 enum ftypes {tNone, tBin, tComp, tChar, tSpc, tSci, tNrm, tTime,
              tInt, tFloat, tFract, tDeg, tString, tPtr} fmt;
 char c, cc;
 char param[16];
 int p = 0;
 int i;
 int n=0;
 bool flag;
 char *dst = dest;
 if (!sfmt) return 0;
 do
  {
   c = '\0'; i = 0; flag = false; fmt = tNone;
   do
    {
     cc = c;
     c = pfmt[i++] = *sfmt++;
     pfmt[i] = '\0';
     if (flag)
      {
       if (c == '%') {flag = false; continue;}
       else
       if ((c == 'f') || (c == 'e') || (c == 'E') ||
           (c == 'g') || (c == 'G'))
        {fmt = tFloat; break;}
       else
       if ((c == 'd') || (c == 'i') || (c == 'u') ||
           (c == 'x') || (c == 'X') || (c == 'o'))
        {fmt = tInt; break;}
       else
       if (c == 'c') {fmt = tChar; break;}
       else
       if (c == 'C') {fmt = tSpc; break;}
       else
       if (c == 'D') {fmt = tDeg; break;}
       else
       if (c == 'S') {fmt = tSci; break;}
       else
       if (c == 'F') {fmt = tFract; break;}
       else
       if (c == 'N') {fmt = tNrm; break;}
       else
       if (c == 't') {fmt = tTime; break;}
       else
       if (c == 'b') {fmt = tBin; break;}
       else
       if (c == 'B') {fmt = tComp; break;}
       else
       if (c == 's') {fmt = tString; break;}
       else
       if ((c == 'n')||(c == 'p')) {fmt = tPtr; break;}
       else
        {
         if (p < sizeof(param)) param[p++] = c;
         param[p] = '\0';
         continue;
        }
      }
     else
     if (c == '%') {flag = true; continue;}
    }
   while (c && (i < STRBUF));
   if (!c) sfmt--; //poit to '\0' ???
   if (n < args)
    {
     switch(fmt)
      {
       case tNone:
        dst += sprintf(dst, pfmt);
       break;
       case tPtr:
        strcpy(dst, pfmt);
        dst += strlen(pfmt);
       break;
       case tComp:
        {
         float_t cd = v_stack[n].get();
         dst += fmtc(dst, pfmt);
         dst += b2scistr(dst, cd);
        }
       break;
       case tBin:
        {
         __int64 bi = v_stack[n].get_int();
         dst += fmtc(dst, pfmt);
         dst += b2str(dst, pfmt, bi);
        }
       break;
       case tChar:
        {
         if (cc == 'l')
          {
           int ii = v_stack[n].get_int();
           dst += fmtc(dst, pfmt);
           dst += wchr2str(dst, ii);
          }
         else
          {
           char ci = v_stack[n].get_int();
           dst += sprintf(dst, pfmt, ci);
          }
        }
       break;
       case tSpc:
        {
         char ci = v_stack[n].get_int();
         dst += fmtc(dst, pfmt);
         dst += chr2str(dst, ci);
        }
       break;
       case tSci:
        {
         double dd = v_stack[n].get();
         dst += fmtc(dst, pfmt);
         dst += d2scistr(dst, dd);
        }
       break;
       case tFract:
        {
         int en = 0;
         double dd = v_stack[n].get();
         dst += fmtc(dst, pfmt);
         if (param[0]) en = atoi(param);
         dst += d2frcstr(dst, dd, en);                 
        }
       break;
       case tNrm:
        {
         double dd = v_stack[n].get();
         dst += fmtc(dst, pfmt);
         dst += d2nrmstr(dst, dd);
        }
       break;
       case tTime:
        {
         __int64 bi = v_stack[n].get_int();
         dst += fmtc(dst, pfmt);
         dst += t2str(dst, bi);
        }
       break;
       case tInt:
        {
         if (cc == 'l')
          {
           long li = v_stack[n].get_int();
           dst += sprintf(dst, pfmt, li);
          }
         else
         if (cc == 'L')
          {
           __int64 Li = v_stack[n].get_int();
           dst += sprintf(dst, pfmt, Li);
          }
         else
         if (cc == 'h')
          {
           short hi = v_stack[n].get_int();
           dst += sprintf(dst, pfmt, hi);
          }
         else
          {
           int ii = v_stack[n].get_int();
           dst += sprintf(dst, pfmt, ii);
          }
        }
       break;
       case tFloat:
        {
         if (cc == 'L')
          {
           long double Ld = v_stack[n].get();
           dst += sprintf(dst, pfmt, Ld);
          }
         else
          {
           double dd = v_stack[n].get();
           dst += sprintf(dst, pfmt, dd);
          }
        }
       break;
       case tDeg:
        {
         double dd = v_stack[n].get();
         dst += fmtc(dst, pfmt);
         dst += dgr2str(dst, dd);
        }
       break;
       case tString:
        dst += sprintf(dst, pfmt, v_stack[n].sval);
       break;
      }
     n++;
    }
   //else dst += sprintf(dst, fmt);
  }
 while (*sfmt && (i < STRBUF) && (n < args));
 return dst-dest;
}

//strftime(char *s, size_t maxsize, const char *fmt, const struct tm *t);
//struct tm *gmtime(const time_t *timer);

int_t datatime(char *tstr)
{                                              
  time_t result = 0;
  int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;

  if (sscanf(tstr, "%4d.%2d.%2d %2d:%2d:%2d", &year, &month, &day, &hour, &min, &sec))
   {
    struct tm breakdown = {0};
    breakdown.tm_year = year - 1900; /* years since 1900 */
    breakdown.tm_mon = month - 1;
    breakdown.tm_mday = day;
    breakdown.tm_hour = hour;
    breakdown.tm_min = min;
    breakdown.tm_sec = sec;
    result = mktime(&breakdown)-_timezone;
   }
 return (int_t)result;
}

static int_t time(char *dest, char *sfmt, int_t time)
{
 //strftime(
 return 0;
}

calculator::calculator(int cfg)
{
  errpos = 0;
  tmp_var_count = 0;
  err[0] = '\0';
  scfg = cfg;
  sres[0] = '\0';
  memset(hash_table, 0, sizeof hash_table);
  memset(v_stack, 0, sizeof v_stack);
  randomize();

  add(tsFFUNC1, "abs", (void*)(float_t(*)(float_t))fabsl);
  //add(tsIFFUNC3, "aeq", (void*)(int_t(*)(float_t, float_t, int_t))nearly_equal);
  add(tsFFUNC1, "erf", (void*)(float_t(*)(float_t))Erf);
  add(tsFFUNC1, "acos", (void*)(float_t(*)(float_t))Acos);
  add(tsFFUNC1, "asin", (void*)(float_t(*)(float_t))Asin);
  add(tsFFUNC1, "atan", (void*)(float_t(*)(float_t))Atan);
  add(tsFFUNC1, "arctg", (void*)(float_t(*)(float_t))Atan);
  add(tsFFUNC2, "atan2", (void*)(float_t(*)(float_t,float_t))Atan2l);
  add(tsFFUNC2, "hypot", (void*)(float_t(*)(float_t,float_t))Hypot);
  add(tsFFUNC1, "cos", (void*)(float_t(*)(float_t))Cos);
  add(tsFFUNC1, "cosh", (void*)(float_t(*)(float_t))Cosh);
  add(tsFFUNC1, "ch", (void*)(float_t(*)(float_t))Cosh);
  add(tsFFUNC1, "exp", (void*)(float_t(*)(float_t))Exp);
  add(tsFFUNC1, "log", (void*)(float_t(*)(float_t))Log);
  add(tsFFUNC1, "log10", (void*)(float_t(*)(float_t))Lg);
  add(tsFFUNC1, "np", (void*)(float_t(*)(float_t))NP);
  add(tsFFUNC1, "db", (void*)(float_t(*)(float_t))DB);
  add(tsFFUNC1, "anp", (void*)(float_t(*)(float_t))ANP);
  add(tsFFUNC1, "adb", (void*)(float_t(*)(float_t))ADB);
  add(tsFFUNC1, "sin", (void*)(float_t(*)(float_t))Sin);
  add(tsFFUNC1, "sinh", (void*)(float_t(*)(float_t))Sinh);
  add(tsFFUNC1, "sh", (void*)(float_t(*)(float_t))Sinh);
  add(tsFFUNC1, "tan", (void*)(float_t(*)(float_t))Tan);
  add(tsFFUNC1, "tanh", (void*)(float_t(*)(float_t))Tanh);
  add(tsFFUNC1, "th", (void*)(float_t(*)(float_t))Tanh);
  add(tsFFUNC1, "sqrt", (void*)(float_t(*)(float_t))Sqrt);
  add(tsFFUNC1, "ctanh", (void*)(float_t(*)(float_t))Ctanh);
  add(tsFFUNC1, "cth", (void*)(float_t(*)(float_t))Ctanh);
  add(tsFFUNC1, "arsh", (void*)(float_t(*)(float_t))Arsh);
  add(tsFFUNC1, "arch", (void*)(float_t(*)(float_t))Arch);
  add(tsFFUNC1, "arth", (void*)(float_t(*)(float_t))Arth);
  add(tsFFUNC1, "arcth", (void*)(float_t(*)(float_t))Arcth);
  add(tsFFUNC1, "float", (void*)To_float);
  add(tsIFUNC1, "int", (void*)To_int);
  add(tsIFUNC2, "gcd", (void*)(int_t(*)(int_t,int_t))Gcd);
  add(tsIFUNC2, "invmod", (void*)(int_t(*)(int_t,int_t))Invmod);
  add(tsIFUNC1, "prime", (void*)Prime);
  add(tsPFUNCn, "fprn", (void*)(int_t(*)(char*, char*, int args, value*))fprn);
  add(tsPFUNCn, "prn", (void*)(int_t(*)(char*, char*, int args, value*))fprn);
  add(tsPFUNCn, "printf", (void*)(int_t(*)(char*, char*, int args, value*))fprn);
  add(tsSIFUNC1, "datatime", (void*)datatime);
  add(tsFFUNC1, "ln", (void*)(float_t(*)(float_t))Log);
  add(tsFFUNC1, "lg", (void*)(float_t(*)(float_t))Lg);
  add(tsFFUNC1, "exp10", (void*)(float_t(*)(float_t))Exp10);
  add(tsFFUNC1, "arcsin", (void*)(float_t(*)(float_t))Asin);
  add(tsFFUNC1, "arccos", (void*)(float_t(*)(float_t))Acos);
  add(tsFFUNC1, "sing", (void*)(float_t(*)(float_t))Sing);
  add(tsFFUNC1, "cosg", (void*)(float_t(*)(float_t))Cosg);
  add(tsFFUNC1, "tg", (void*)(float_t(*)(float_t))Tan);
  add(tsFFUNC1, "root2", (void*)(float_t(*)(float_t))Sqrt);
  add(tsFFUNC2, "pow", (void*)(float_t(*)(float_t,float_t))Pow);
  add(tsFFUNC1, "tgg", (void*)(float_t(*)(float_t))Tgg);
  add(tsFFUNC1, "ctg", (void*)(float_t(*)(float_t))Ctg);
  add(tsFFUNC1, "ctgg", (void*)(float_t(*)(float_t))Ctgg);
  add(tsFFUNC1, "frac", (void*)(float_t(*)(float_t))Frac);
  add(tsFFUNC1, "round", (void*)(float_t(*)(float_t))Round);
  add(tsIFUNC1, "not", (void*)Not);
  add(tsIFUNC1, "now", (void*)Now);
  add(tsFFUNC2, "min", (void*)(float_t(*)(float_t,float_t))Min);
  add(tsFFUNC2, "max", (void*)(float_t(*)(float_t,float_t))Max);
  add(tsFFUNC1, "log2", (void*)(float_t(*)(float_t))Log2);
  add(tsFFUNC1, "fact", (void*)(float_t(*)(float_t))Factorial);
  add(tsFFUNC2, "logn", (void*)(float_t(*)(float_t,float_t))Logn);
  add(tsFFUNC1, "root3", (void*)(float_t(*)(float_t))Root3);
  add(tsFFUNC1, "cbrt", (void*)(float_t(*)(float_t))Root3);
  add(tsFFUNC2, "rootn", (void*)(float_t(*)(float_t,float_t))Rootn);
  add(tsFFUNC1, "swg", (void*)(float_t(*)(float_t))Swg);
  add(tsFFUNC1, "sswg", (void*)(float_t(*)(float_t))SSwg);
  add(tsFFUNC1, "aswg", (void*)(float_t(*)(float_t))Aswg);
  add(tsFFUNC1, "awg", (void*)(float_t(*)(float_t))Awg);
  add(tsFFUNC1, "sawg", (void*)(float_t(*)(float_t))SAwg);
  add(tsFFUNC1, "aawg", (void*)(float_t(*)(float_t))Aawg);
  add(tsFFUNC1, "cs", (void*)(float_t(*)(float_t))Cs);
  add(tsFFUNC1, "acs", (void*)(float_t(*)(float_t))Acs);
  add(tsFFUNC1, "rnd", (void*)(float_t(*)(float_t))Random);
  add(tsFFUNC3, "vout", (void*)(float_t(*)(float_t, float_t, float_t))Vout);


  add(tsFFUNC2, "ee", (void*)(float_t(*)(float_t,float_t))Ee);

  addfvar("pi", M_PI);
  addfvar("e", M_E);
  addivar("max32", 0x7fffffff);
  addivar("maxint", 0x7fffffff);
  addivar("maxu32", 0xffffffff);
  addivar("maxuint", 0xffffffff);
  addivar("max64", 0x7fffffffffffffffL);
  addivar("maxlong", 0x7fffffffffffffffL);
  addivar("maxu64", 0xffffffffffffffffL);
  addivar("maxulong", 0xffffffffffffffffL);
  addivar("timezone", -_timezone/3600);
  addivar("daylight", _daylight);
  addivar("tz", _daylight-_timezone/3600);
}

calculator::~calculator(void)
{
 symbol* sp;
 symbol* nsp;

 for(int i = 0; i < hash_table_size; i++)
  {
   if ((sp = hash_table[i]) != NULL)
    {
     do
      {
       nsp = sp->next;
       free(sp->name);
       delete sp;
       sp = nsp;
       hash_table[i] = NULL;
      }
     while (nsp);
    }
  }
}

void calculator::varlist(void (*f)(char*, float_t))
{
 symbol* sp;
 for (int i = 0; i < hash_table_size; i++)
  {
   if ((sp = hash_table[i]) != NULL)
    {
     do
      {
       if ((f) && (sp->tag == tsVARIABLE))
        {
         f(sp->name, (float_t)sp->val.get());
        }
       sp = sp->next;
      }
     while (sp);
    }
  }
}

void calculator::varlist(void (*f)(char*, value*))
{
  symbol* sp;
  for (int i = 0; i < hash_table_size; i++)
    {
      if ((sp = hash_table[i]) != NULL)
        {
          do
            {
              if ((f) && (sp->tag == tsVARIABLE))
               {
                f(sp->name, &sp->val);
               }
              sp = sp->next;
            }
          while (sp);
        }
    }
}

unsigned calculator::string_hash_function(char* p)
{
  unsigned h = 0, g;
  while (*p)
    {
      if (scfg & UPCASE) h = (h << 4) + toupper(*p++);
      else h = (h << 4) + *p++;

      if ((g = h & 0xF0000000) != 0)
        {
          h ^= g >> 24;
        }
      h &= ~g;
    }
  return h;
}

symbol* calculator::add(t_symbol tag, const char* name, void* func)
{
  char *uname = strdup(name);

  unsigned h = string_hash_function(uname) % hash_table_size;
  symbol* sp;
  for (sp = hash_table[h]; sp != NULL; sp = sp->next)
    {
      if (scfg & UPCASE)
       {
#ifdef _WIN_
        if (stricmp(sp->name, uname) == 0) return sp;
#else  /*_WIN_*/
	if (strcasecmp(sp->name, uname) == 0) return sp;
#endif /*_WIN_*/
       }
      else
       {
        if (strcmp(sp->name, uname) == 0) return sp;
       }
    }
  sp = new symbol;
  sp->tag = tag;
  sp->func = func;
  sp->name = uname;
  sp->val.tag = tvINT;
  sp->val.ival = 0;
  sp->next = hash_table[h];
  hash_table[h] = sp;
  return sp;
}

void calculator::addfvar(const char* name, float_t val)
{
 symbol* sp = add(tsVARIABLE, name);
 sp->val.tag = tvFLOAT;
 sp->val.fval = val;
}

bool calculator::checkvar(const char* name)
{
  char *uname = strdup(name);

  unsigned h = string_hash_function(uname) % hash_table_size;
  symbol* sp;
  for (sp = hash_table[h]; sp != NULL; sp = sp->next)
    {
      if (scfg & UPCASE)
       {
#ifdef _WIN_
        if (stricmp(sp->name, uname) == 0) return true;
#else  /*_WIN_*/
	if (strcasecmp(sp->name, uname) == 0) return true;
#endif /*_WIN_*/
       }
      else
       {
        if (strcmp(sp->name, uname) == 0) return true;
       }
    }
  return false;
}

void calculator::addivar(const char* name, int_t val)
{
 symbol* sp = add(tsVARIABLE, name);
 sp->val.tag = tvINT;
 sp->val.ival = val;
}

int calculator::hscanf(char* str, int_t &ival, int &nn)
{
 int_t res = 0;
 char c;
 int n = 0;
 while (c = *str++, c && (n < 16))
  {
   if ((c >= '0') && (c <= '9')) {res = res * 16 + (c - '0'); n++;}
   else
   if ((c >= 'A') && (c <= 'F')) {res = res * 16 + (c - 'A') + 0xA; n++;}
   else
   if ((c >= 'a') && (c <= 'f')) {res = res * 16 + (c - 'a') + 0xa; n++;}
   else break;
  }
 ival = res;
 nn = n;
 if (n) scfg |= HEX;
 return 0;
}

int calculator::bscanf(char* str, int_t &ival, int &nn)
{
 int_t res = 0;
 char c;
 int n = 0;

 while (c = *str++, c && (n < 64))
  {
   if ((c >= '0') && (c <= '1')) {res = res * 2 + (c - '0'); n++;}
   else break;
  }
 ival = res;
 nn = n;
 if (n) scfg |= fBIN;
 return 0;
}

int calculator::oscanf(char* str, int_t &ival, int &nn)
{
 int_t res = 0;
 char c;
 int n = 0;

 while (c = *str++, c && (n < 24))
  {
   if ((c >= '0') && (c <= '7')) {res = res * 8 + (c - '0'); n++;}
   else break;
  }
 ival = res;
 nn = n;
 if (n) scfg |= OCT;
 return 0;
}

int calculator::xscanf(char* str, int len, int_t &ival, int &nn)
{
 int_t res = 0;
 char c;
 int n=0;
 int hmax, omax;
 int max;

 switch (len)
  {
   case 1:
    max = 0x100;
    hmax = 3;
    omax = 3;
   break;
   case 2:
    max = 0x10000;
    hmax = 5;
    omax = 6;
   break;
   default: max = 0;
  }
 switch (*str)
  {
    case '0':  case '1':  case '2': case '3':
     {
      while (c = *str++, c && (n < omax))
      {
       if ((c >= '0') && (c <= '7')) {res = res * 8 + (c - '0'); n++;}
       else break;
      }
     if (res >= max) n--;
     if (n) scfg |= OCT;
     }
    break;

    case 'x':
    case 'X':
     str++; n++;
     while (c = *str++, c && (res < max) && (n < hmax))
      {
       if ((c >= '0') && (c <= '9')) {res = res * 16 + (c - '0'); n++;}
       else
       if ((c >= 'A') && (c <= 'F')) {res = res * 16 + (c - 'A') + 0xA; n++;}
       else
       if ((c >= 'a') && (c <= 'f')) {res = res * 16 + (c - 'a') + 0xa; n++;}
       else break;
      }
     if (res >= max) n--;
     if (n) scfg |= HEX;
    break;

    case 'a':
     res = '\007'; n = 1;
     scfg |= ESC;
    break;

    case 'f':
     res = 255u; n = 1;
     scfg |= ESC;
    break;

    case 'v':
     res = '\x0b'; n = 1;
     scfg |= ESC;
    break;

    case 'E':  case 'e':
     res = '\033'; n = 1;
     scfg |= ESC;
    break;

    case 't':
     res = '\t'; n = 1;
     scfg |= ESC;
    break;

    case 'n':
     res = '\n'; n = 1;
     scfg |= ESC;
    break;

    case 'r':
     res = '\r'; n = 1;
     scfg |= ESC;
    break;

    case 'b':
     res = '\b'; n = 1;
     scfg |= ESC;
    break;

    case '\\':
     res = '\\'; n = 1;
     scfg |= ESC;
    break;
  }
 ival = res;
 nn = n;
 return 0;
}


float_t calculator::dstrtod(char *s, char **endptr)
{
 const char cdeg[] = {'\`', '\'', '\"'}; //` - degrees, ' - minutes, " - seconds
 const float_t mdeg[] = {M_PI/180.0, M_PI/(180.0*60), M_PI/(180.0*60*60)};
 float_t res = 0;
 float_t d;
 char* end = s;

 for(int i = 0; i < 3; i++)
  {
   d = strtod(end, &end);
   do
    {
     if (*end == cdeg[i])
      {
       res += d * mdeg[i];
       end++;
       scfg |= DEG;
       break;
      }
     else i++;
    }
   while (i < 3);
  }
 *endptr = end;
 return res;
}

//1:c1:y1:d1:h1:m1:s  => 189377247661s
float_t calculator::tstrtod(char *s, char **endptr)
{
 const float_t dms[] =
   {(60.0*60.0*60.0*24.0*365.25*100.0),(60.0*60.0*24.0*365.25),
    (60.0*60.0*24.0), (60.0*60.0), 60.0, 1.0};
 const char cdt[] =  {'c', 'y', 'd', 'h', 'm', 's'};
 float_t res = 0;
 float_t d;
 char* end = s;

 for(int i = 0; i < 6; i++)
  {
   d = strtod(end, &end);
   do
    {
     if ((*end == ':') && (*(end+1) == cdt[i]))
      {
       res += d * dms[i];
       end += 2;
       scfg |= DAT;
       break;
      }
     else i++;
    }
   while (i < 6);
  }
 *endptr = end;
 return res;
}

// http://searchstorage.techtarget.com/sDefinition/0,,sid5_gci499008,00.html
// process expression like 1k56 => 1.56k (maximum 3 digits)
void calculator::engineering(float_t mul, char * &fpos, float_t &fval)
{
 int fract = 0;
 int div = 1;
 int n = 3; //maximum 3 digits
 while(*fpos && (*fpos >= '0') && ((*fpos <= '9')) && n--)
  {
   div *= 10;
   fract *= 10;
   fract += *fpos++-'0';
   scfg |= ENG;
  }
 fval *= mul;
 fval += (fract*mul)/div;
 scfg |= SCF;
}

void calculator::scientific(char * &fpos, float_t &fval)
 {
  if (*(fpos-1) == 'E') fpos--;
  switch (*fpos)
    {
     case '\"': //Inch
      if (scfg & FRI)
       {
        fpos++;
        //fval *= 25.4e-3;
        engineering(25.4e-3, fpos, fval);
       }
     break;
     case 'Y':
       fpos++;
       if (*fpos == 'B')
        {
         fpos++;
         fval *= 1.20892582e+24;  //2**80
         scfg |= CMP;
        }
       else engineering(1e24, fpos, fval);
       break;
     case 'Z':
       fpos++;
       if (*fpos == 'B')
        {
         fpos++;
	 fval *= 1.180591620717411e+21;  //2**70
         scfg |= CMP;
        }
       else engineering(1e21, fpos, fval);
       break;
     case 'E':
       fpos++;
       if (*fpos == 'B')
        {
         fpos++;
         fval *= 1152921504606846976i64; //2**60
         scfg |= CMP;
        }
       else engineering(1e18, fpos, fval);
       break;
     case 'P':
       fpos++;
       if (*fpos == 'B')
        {
         fpos++;
         fval *= 1125899906842624i64; //2**50
         scfg |= CMP;
        }
       else engineering(1e15, fpos, fval);
       break;
     case 'T':
       fpos++;
       if (*fpos == 'B')
        {
         fpos++;
         fval *= 1099511627776i64; //2**40
         scfg |= CMP;
        }
       else engineering(1e12, fpos, fval);
       break;
     case 'G':
       fpos++;
       if (*fpos == 'B')
        {
         fpos++;
         fval *= 1073741824i64;  //2**30
         scfg |= CMP;
        }
       else engineering(1e9, fpos, fval);
       break;
     case 'M':
       fpos++;
       if (*fpos == 'B')
        {
         fpos++;
         fval *= 1048576;  //2**20
         scfg |= CMP;
        }
       else engineering(1e6, fpos, fval);
       break;
     case 'K':
       fpos++;
       if (*fpos == 'B')
        {
         fpos++;
         fval *= 1024;  //2**10
         scfg |= CMP;
        }
       else engineering(1e3, fpos, fval);
       break;
     case 'R':
       fpos++;
       engineering(1, fpos, fval);
       break;
     case 'h':
       fpos++;
       engineering(1e2, fpos, fval);
       break;
     case 'k':
       fpos++;
       engineering(1e3, fpos, fval);
       break;
     case 'D':
       fpos++;
       engineering(1e1, fpos, fval);
       break;
     case 'd':
       fpos++;
       if (*fpos == 'a')
        {
         fpos++;
         engineering(1e1, fpos, fval);
        }
       else engineering(1e-1, fpos, fval);
       break;
     case 'c':
       fpos++;
       engineering(1e-2, fpos, fval);
       break;
     case 'm':
       fpos++;
       engineering(1e-3, fpos, fval);
       break;
     case 'u':
       fpos++;
       engineering(1e-6, fpos, fval);
       break;
     case 'n':
       fpos++;
       engineering(1e-9, fpos, fval);
       break;
     case 'p':
       fpos++;
       engineering(1e-12, fpos, fval);
       break;
     case 'f':
       fpos++;
       engineering(1e-15, fpos, fval);
       break;
     case 'a':
       fpos++;
       engineering(1e-18, fpos, fval);
       break;
     case 'z':
       fpos++;
       engineering(1e-21, fpos, fval);
       break;
     case 'y':
       fpos++;
       engineering(1e-24, fpos, fval);
       break;
    }
 }

void calculator::error(int pos, const char* msg)
{
  sprintf(err, "Error: %s at %i\n\n", msg, pos);
  errpos = pos;
}

t_operator calculator::scan(bool operand, bool percent)
{
  char name[max_expression_length], *np;

  while (isspace(buf[pos])) pos += 1;
  switch (buf[pos++])
    {
    case '\0':
      return toEND;
    case '(':
      return toLPAR;
    case ')':
      return toRPAR;
    case '+':
      if (buf[pos] == '+')
        {
          pos += 1;
          return operand ? toPREINC : toPOSTINC;
        }
      else
      if (buf[pos] == '=')
        {
          pos += 1;
          return toSETADD;
        }
      return operand ? toPLUS : toADD;
    case '-':
      if (buf[pos] == '-')
        {
          pos += 1;
          return operand ? toPREDEC : toPOSTDEC;
        }
      else
      if (buf[pos] == '=')
        {
          pos += 1;
          return toSETSUB;
        }
      return operand ? toMINUS : toSUB;
    case '!':
      if (buf[pos] == '=')
        {
          pos += 1;
          return toNE;
        }
      return operand ? toNOT : toFACT;
    case '~':
      return toCOM;
    case ';':
      return toSEMI;
    case '*':
      if (buf[pos] == '*')
        {
          if (buf[pos+1] == '=')
            {
              pos += 2;
              return toSETPOW;
            }
          pos += 1;
          return toPOW;
        }
      else
      if (buf[pos] == '=')
        {
          pos += 1;
          return toSETMUL;
        }
      return toMUL;
    case '/':
      if (buf[pos] == '=')
        {
          pos += 1;
          return toSETDIV;
        }
      else
      if (buf[pos] == '/')
        {
          pos += 1;
          return toPAR;
        }
      return toDIV;
    case '%':
      if (buf[pos] == '=')
        {
          pos += 1;
          return toSETMOD;
        }
      else
      if (buf[pos] == '%')
        {
          pos += 1;
          return toPERCENT;
        }
      return toMOD;
    case '<':
      if (buf[pos] == '<')
        {
          if (buf[pos+1] == '=')
            {
              pos += 2;
              return toSETASL;
            }
          else
            {
              pos += 1;
              return toASL;
            }
        }
      else
      if (buf[pos] == '=')
        {
          pos += 1;
          return toLE;
        }
      else
      if (buf[pos] == '>')
        {
          pos += 1;
          return toNE;
        }
      return toLT;
    case '>':
      if (buf[pos] == '>')
        {
          if (buf[pos+1] == '>')
            {
              if (buf[pos+2] == '=')
                {
                  pos += 3;
                  return toSETLSR;
                }
              pos += 2;
              return toLSR;
            }
          else
          if (buf[pos+1] == '=')
            {
              pos += 2;
              return toSETASR;
            }
          else
            {
              pos += 1;
              return toASR;
            }
        }
      else
      if (buf[pos] == '=')
        {
          pos += 1;
          return toGE;
        }
      return toGT;
    case '=':
      if (buf[pos] == '=')
        {
          scfg &= ~PAS;
          pos += 1;
          return toEQ;
        }
      if (scfg & PAS) return toEQ;
      else return toSET;
    case ':':
      if (buf[pos] == '=')
        {
          scfg |= PAS;
          pos += 1;
          return toSET;
        }
      error("syntax error");
      return toERROR;
    case '&':
      if (buf[pos] == '&')
        {
          pos += 1;
          return toAND;
        }
      else
      if (buf[pos] == '=')
        {
          pos += 1;
          return toSETAND;
        }
      return toAND;
    case '|':
      if (buf[pos] == '|')
        {
          pos += 1;
          return toOR;
        }
      else
      if (buf[pos] == '=')
        {
          pos += 1;
          return toSETOR;
        }
      return toOR;
    case '^':
      if (scfg & PAS)
       {
        if (buf[pos] == '=')
          {
            pos += 1;
            return toSETPOW;
          }
        return toPOW;
       }
      else
       {
        if (buf[pos] == '=')
          {
            pos += 1;
            return toSETXOR;
          }
        return toXOR;
       }
    case '#':
     if (operand)
      {
       float_t fval;
       char *fpos;
       if (buf[pos])
        {
         fval = Awg(strtod(buf+pos, &fpos));
         v_stack[v_sp].tag = tvFLOAT;
         v_stack[v_sp].fval = fval;
         pos = fpos - buf;
         v_stack[v_sp].pos = pos;
         v_stack[v_sp++].var = NULL;
         return toOPERAND;
        }
       else
        {
         error("bad numeric constant");
         return toERROR;
        }
      }
     else
      {
       if (buf[pos] == '=')
         {
           pos += 1;
           return toSETXOR;
         }
       return toXOR;
      }
    case ',':
      return toCOMMA;
    case '\'':
     {
      int_t ival;
      char *ipos, *fpos;
      wchar_t wc;
      int n = 0;

      if (buf[pos] == '\\')
       {
        xscanf(buf+pos+1, 1, ival, n);
        ipos = buf+pos+n+1;
        if (*ipos == '\'') ipos++;
        else
         {
           error("bad char constant");
           return toERROR;
         }
       }
      else
       {
         ipos = buf+pos+1;
         if (*ipos == '\'')
          {
#ifdef _WCHAR_
#ifdef _WIN_
           if (*(ipos+1) == 'W')
            {
             wchar_t wbuf[2];
             char cbuf[2];

             cbuf[0] = *(ipos-1);
             cbuf[1] = '\0';

             MultiByteToWideChar(CP_OEMCP, 0, (LPSTR)cbuf, -1,
                    (LPWSTR)wbuf, 2);
             ival = *(int*)&wbuf[0];
             ipos+=2;
             scfg |= WCH;
            }
           else
#endif  /*_WIN_*/
#endif /*_WCHAR_*/
            {
             scfg |= CHR;
             ival = *(unsigned char *)(ipos-1);
             v_stack[v_sp].sval = (char *) malloc(STRBUF);
             v_stack[v_sp].sval[0] = *(ipos-1);
             v_stack[v_sp].sval[1] = '\0';
             ipos++;
            }
          }
         else
          {
           char sbuf[STRBUF];
           int sidx = 0;
           ipos = buf+pos;
           while (*ipos && (*ipos != '\'') && (sidx < STRBUF))
            sbuf[sidx++] = *ipos++;
           sbuf[sidx] = '\0';
           if (*ipos == '\'')
            {
             if (sbuf[0]) scfg |= STR;
             v_stack[v_sp].tag = tvSTR;
             v_stack[v_sp].ival = 0;
             v_stack[v_sp].sval = (char *)malloc(STRBUF);
             strcpy(v_stack[v_sp].sval, sbuf);
             pos = ipos - buf+1;
             v_stack[v_sp].pos = pos;
             v_stack[v_sp++].var = NULL;
             return toOPERAND;
            }
           else
            {
             error("bad char constant");
             return toERROR;
            }
          }
       }
      v_stack[v_sp].tag = tvINT;
      v_stack[v_sp].ival = ival;
      pos = ipos - buf;
      v_stack[v_sp].pos = pos;
      v_stack[v_sp++].var = NULL;
      return toOPERAND;
     }
#ifdef _WCHAR_
#ifdef _WIN_
    case 'L':
     {
      int_t ival;
      char *ipos, *fpos;
      wchar_t wc;
      int n = 0;

      if (buf[pos] == '\'')
       {
        if (buf[pos+1] == '\\')
         {
          xscanf(buf+pos+2, 2, ival, n);
          ipos = buf+pos+n+2;
          if (*ipos == '\'') ipos++;
          else
           {
            error("bad char constant");
            return toERROR;
           }
         }
        else
         {
           ipos = buf+pos;
           if (*(ipos+2) == '\'')
            {
             wchar_t wbuf[2];
             char cbuf[2];

             cbuf[0] = *(ipos+1);
             cbuf[1] = '\0';

             MultiByteToWideChar(CP_OEMCP, 0, (LPSTR)cbuf, -1,
                    (LPWSTR)wbuf, 2);
             ival = *(int*)&wbuf[0];
             ipos+=3;
             scfg |= WCH;
            }
           else
            {
             error("bad char constant");
             return toERROR;
            }
         }
        v_stack[v_sp].tag = tvINT;
        v_stack[v_sp].ival = ival;
        pos = ipos - buf;
        v_stack[v_sp].pos = pos;
        v_stack[v_sp++].var = NULL;
        return toOPERAND;
       }
      goto def;
     }
#endif /*_WIN_*/
#endif /*_WCHAR_*/
    case '"':
     {
       char *ipos, *fpos;
       char sbuf[STRBUF];
       int sidx = 0;
       ipos = buf+pos;
       while (*ipos && (*ipos != '"') && (sidx < STRBUF))
        sbuf[sidx++] = *ipos++;
       sbuf[sidx] = '\0';
       if (*ipos == '"')
        {
         if (sbuf[0]) scfg |= STR;
         v_stack[v_sp].tag = tvSTR;
         v_stack[v_sp].ival = 0;
         v_stack[v_sp].sval = (char *)malloc(STRBUF);
         strcpy(v_stack[v_sp].sval, sbuf);
         pos = ipos - buf+1;
         v_stack[v_sp].pos = pos;
         v_stack[v_sp++].var = NULL;
         return toOPERAND;
        }
       else
        {
         error("bad char constant");
         return toERROR;
        }
     }
    case '.': case '0': case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9': case '\\': case '$':
     {
      int_t ival;
      float_t fval;
      int ierr = 0, ferr;
      char *ipos, *fpos;
      int n = 0;

      if (buf[pos-1] == '\\')
       {
        ierr = xscanf(buf+pos, 1, ival, n);
        ipos = buf+pos+n;
       }
      else
      if ((buf[pos-1] == '0') && ((buf[pos] == 'B') || (buf[pos] == 'b')))
       {
        ierr = bscanf(buf+pos+1, ival, n);
        ipos = buf+pos+n+1;
       }
      else
      if ((buf[pos-1] == '0') && ((buf[pos] == 'O') || (buf[pos] == 'o')))
       {
        ierr = oscanf(buf+pos+1, ival, n);
        ipos = buf+pos+n+1;
       }
      else
      if (buf[pos-1] == '$')
       {
        ierr = hscanf(buf+pos, ival, n);
        ipos = buf+pos+n;
       }
      else
      if (buf[pos-1] == '0')
       {
        ierr = sscanf(buf+pos-1, "%" INT_FORMAT "i%n", &ival, &n) != 1;
        ipos = buf+pos-1+n;
        if ((ierr==0)&&((buf[pos] == 'x')||(buf[pos] == 'X'))) scfg |= HEX;
       }
      else
       {
        if (scfg & FFLOAT) {ierr = 0; n = 0;}
        else ierr = sscanf(buf+pos-1, "%" INT_FORMAT "i%n", &ival, &n) != 1;
        ipos = buf+pos-1+n;
       }
      errno = 0;
      fval = strtod(buf+pos-1, &fpos);

      //` - degrees, ' - minutes, " - seconds
      if ((*fpos == '\'') || (*fpos == '\`') || (((scfg & FRI)==0)&&(*fpos == '\"')))
        fval = dstrtod(buf+pos-1, &fpos);
      else
      if (*fpos == ':') fval = tstrtod(buf+pos-1, &fpos);
      else
      if (scfg & SCI+FRI) scientific(fpos, fval);
      ferr = errno;
      if (ierr && ferr)
       {
        error("bad numeric constant");
        return toERROR;
       }
      if (v_sp == max_stack_size)
       {
        error("stack overflow");
        return toERROR;
       }                              
      if (!ierr && ipos >= fpos)
       {
        v_stack[v_sp].tag = tvINT;
        v_stack[v_sp].ival = ival;
        pos = ipos - buf;
       }
      else
       {
        if (operand && percent && (*fpos == '%'))
         {
          fpos++;
          v_stack[v_sp].tag = tvPERCENT;
         }
        else v_stack[v_sp].tag = tvFLOAT;
        v_stack[v_sp].fval = fval;
        pos = fpos - buf;
       }
      v_stack[v_sp].pos = pos;
      v_stack[v_sp++].var = NULL;
      return toOPERAND;
     }
    default:
    def:
      pos -= 1;
      np = name;
      while (isalnum(buf[pos]) || buf[pos] == '@' ||
             buf[pos] == '_' || buf[pos] == '?')
        {
          *np++ = buf[pos++];
        }
      if (np == buf)
        {
          error("Bad character");
          return toERROR;
        }
      *np = '\0';
      symbol* sym = add(tsVARIABLE, name);
      if (v_sp == max_stack_size)
        {
          error("stack overflow");
          return toERROR;
        }
      v_stack[v_sp] = sym->val;
      v_stack[v_sp].pos = pos;
      v_stack[v_sp++].var = sym;
      return (sym->tag == tsVARIABLE) ? toOPERAND : toFUNC;
    }
}

static int lpr[toTERMINALS] =
{
  2, 0, 0, 0,       // BEGIN, OPERAND, ERROR, END,
  4, 4,             // LPAR, RPAR
  5, 98, 98, 98,    // FUNC, POSTINC, POSTDEC, FACT
  98, 98, 98, 98, 98, 98, // PREINC, PREDEC, PLUS, MINUS, NOT, COM,
  90,               // POW,
  80, 80, 80, 80, 80,   // toPERCENT, MUL, DIV, MOD, PAR
  70, 70,           // ADD, SUB,
  60, 60, 60,       // ASL, ASR, LSR,
  50, 50, 50, 50,   // GT, GE, LT, LE,
  40, 40,           // EQ, NE,
  38,               // AND,
  36,               // XOR,
  34,               // OR,
  20, 20, 20, 20, 20, 20, 20, //SET, SETADD, SETSUB, SETMUL, SETDIV, SETMOD,
  20, 20, 20, 20, 20, 20, // SETASL, SETASR, SETLSR, SETAND, SETXOR, SETOR,
  8,               // SEMI
  10               // COMMA
};

static int rpr[toTERMINALS] =
{
  0, 0, 0, 1,       // BEGIN, OPERAND, ERROR, END,
  110, 3,           // LPAR, RPAR
  120, 99, 99, 99,  // FUNC, POSTINC, POSTDEC, FACT
  99, 99, 99, 99, 99, 99, // PREINC, PREDEC, PLUS, MINUS, NOT, COM,
  95,               // POW,
  80, 80, 80, 80, 80,   // toPERCENT, MUL, DIV, MOD, PAR
  70, 70,           // ADD, SUB,
  60, 60, 60,       // ASL, ASR, LSR,
  50, 50, 50, 50,   // GT, GE, LT, LE,
  40, 40,           // EQ, NE,
  38,               // AND,
  36,               // XOR,
  34,               // OR,
  25, 25, 25, 25, 25, 25, 25, //SET, SETADD, SETSUB, SETMUL, SETDIV, SETMOD,
  25, 25, 25, 25, 25, 25, // SETASL, SETASR, SETLSR, SETAND, SETXOR, SETOR,
  10,              // SEMI
  15               // COMMA
};


bool calculator::assign()
{
 value& v = v_stack[v_sp-1];
 if (v.var == NULL)
  {
   error(v.pos, "variable expected");
   return false;
  }
 else
  {
   v.var->val = v;
   return true;
  }
}

float_t calculator::evaluate(char* expression, __int64 * piVal)
{
  char var_name[16];
  bool operand = true;
  bool percent = false;
  int n_args = 0;
  const __int64 i64maxdbl = 0x7feffffffffffffeui64;
  const __int64 i64mindbl = 0x0010000000000001ui64;
  const double maxdbl = *(double*)&i64maxdbl;
  const double mindbl = *(double*)&i64mindbl;
  const float_t qnan = 0.0/0.0;

  buf = expression;
  v_sp = 0;
  o_sp = 0;
  pos = 0;
  err[0] = '\0';
  o_stack[o_sp++] = toBEGIN;

  memset(sres, 0, STRBUF);
  while (true)
    {
     next_token:
      int op_pos = pos;
      int oper = scan(operand, percent);
      if (oper == toERROR)
       {
        return qnan;
       }
      switch(oper)
       {
        case toMUL:
        case toDIV:
        case toMOD:
        case toPOW:
        case toPAR:
        case toADD:
        case toSUB:
        //case toPERCENT:
         percent = true;
        break;
        default:
         percent = false;
       }
      if (!operand)
       {
        if (!BINARY(oper) && oper != toEND && oper != toPOSTINC
            && oper != toPOSTDEC && oper != toRPAR && oper != toFACT)
         {
          error(op_pos, "operator expected");
          return qnan;
         }
        if (oper != toPOSTINC && oper != toPOSTDEC && oper != toRPAR
            && oper != toFACT)
         {
          operand = true;
         }
       }
      else
       {
        if (oper == toOPERAND)
         {
          operand = false;
          n_args += 1;
          continue;
         }
        if (BINARY(oper) || oper == toRPAR)
         {
          error(op_pos, "operand expected");
          return qnan;
         }
       }
      n_args = 1;
      while (lpr[o_stack[o_sp-1]] >= rpr[oper])
        {
          int cop = o_stack[--o_sp];
          if (BINARY(cop) && (v_sp < 2))
           {
            error("Unexpected end of expression");
            return qnan;
           }

          switch (cop)
           {
            case toBEGIN:
              if (oper == toRPAR)
                {
                  error("Unmatched ')'");
                  return qnan;
                }
              if (oper != toEND) error("Unexpected end of input");
              if (v_sp == 1)
                {
                  if (scfg & UTMP)
                   {
                    sprintf(var_name, "@%d", ++tmp_var_count);
                    add(tsVARIABLE, var_name)->val = v_stack[0];
                   }
                  if (v_stack[0].tag == tvINT)
                    {
                     if (piVal) *piVal = v_stack[0].ival;
                     if (v_stack[0].sval)
                      {
                       strcpy(sres, v_stack[0].sval);
                       free(v_stack[0].sval);
                       v_stack[0].sval = NULL;
                      }
                     else sres[0] = '\0';
                     return v_stack[0].ival;
                    }
                  else
                    {
                     if (piVal) *piVal = (__int64)v_stack[0].fval;
                     if (v_stack[0].sval)
                      {
                       strcpy(sres, v_stack[0].sval);
                       free(v_stack[0].sval);
                       v_stack[0].sval = NULL;
                      }
                     else sres[0] = '\0';
                     return v_stack[0].fval;
                    }
                }
              else
              if (v_sp != 0) error("Unexpected end of expression");
              return qnan;

            case toCOMMA:
              n_args += 1;
              continue;

            case toSEMI:
              if ((v_stack[v_sp-1].tag == tvINT) &&
                  (v_stack[v_sp-2].tag == tvINT))
                {
                 v_stack[v_sp-2].ival = v_stack[v_sp-1].ival;
                }
              else
              if ((v_stack[v_sp-1].tag == tvSTR) &&
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                 strcpy(v_stack[v_sp-2].sval, v_stack[v_sp-1].sval);
                }
              else
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                 error(v_stack[v_sp-2].pos, "Illegal string operation");
                 return qnan;
                }
              else
                {
                 v_stack[v_sp-2].fval = v_stack[v_sp-1].get();
                 v_stack[v_sp-2].tag = tvFLOAT;
                }
              v_sp -= 1;
              v_stack[v_sp-1].var = NULL;
              break;

            case toADD:
            case toSETADD:
              if ((v_stack[v_sp-1].tag == tvINT) &&
                  (v_stack[v_sp-2].tag == tvINT))
                {
                 v_stack[v_sp-2].ival += v_stack[v_sp-1].ival;
                }
              else
              if ((v_stack[v_sp-1].tag == tvSTR) &&
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                 strcat(v_stack[v_sp-2].sval, v_stack[v_sp-1].sval);
                }
              else
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                 error(v_stack[v_sp-2].pos, "Illegal string operation");
                 return qnan;
                }
              else
                {
                 if (v_stack[v_sp-1].tag == tvPERCENT)
                  {
                   float_t left = v_stack[v_sp-2].get();
                   float_t right = v_stack[v_sp-1].get();
                   v_stack[v_sp-2].fval = left+(left*right/100.0);
                  }
                 else v_stack[v_sp-2].fval =
                    v_stack[v_sp-2].get() + v_stack[v_sp-1].get();

                 v_stack[v_sp-2].tag = tvFLOAT;
                }
              v_sp -= 1;
              if (cop == toSETADD)
               {
                if (!assign()) return qnan;
               }
              v_stack[v_sp-1].var = NULL;
              break;

            case toSUB:
            case toSETSUB:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
               {
                error(v_stack[v_sp-2].pos, "Illegal string operation");
                return qnan;
               }
              else
              if ((v_stack[v_sp-1].tag == tvINT) &&
                  (v_stack[v_sp-2].tag == tvINT))
               {
                v_stack[v_sp-2].ival -= v_stack[v_sp-1].ival;
               }
              else
               {
                if (v_stack[v_sp-1].tag == tvPERCENT)
                 {
                  float_t left = v_stack[v_sp-2].get();
                  float_t right = v_stack[v_sp-1].get();
                  v_stack[v_sp-2].fval = left-(left*right/100.0);
                 }
                else v_stack[v_sp-2].fval =
                   v_stack[v_sp-2].get() - v_stack[v_sp-1].get();
                v_stack[v_sp-2].tag = tvFLOAT;
               }
              v_sp -= 1;
              if (cop == toSETSUB)
               {
                if (!assign()) return qnan;
               }
              v_stack[v_sp-1].var = NULL;
              break;

            case toMUL:
            case toSETMUL:
              if ((v_stack[v_sp-1].tag == tvINT) &&
                  (v_stack[v_sp-2].tag == tvINT))
               {
                v_stack[v_sp-2].ival *= v_stack[v_sp-1].ival;
               }
              else
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
               {
                error(v_stack[v_sp-2].pos, "Illegal string operation");
                return qnan;
               }
              else
               {
                if (v_stack[v_sp-1].tag == tvPERCENT)
                 {
                  float_t left = v_stack[v_sp-2].get();
                  float_t right = v_stack[v_sp-1].get();
                  v_stack[v_sp-2].fval = left*(left*right/100.0);
                 }
                else v_stack[v_sp-2].fval =
                   v_stack[v_sp-2].get() * v_stack[v_sp-1].get();
                v_stack[v_sp-2].tag = tvFLOAT;
               }
              v_sp -= 1;
              if (cop == toSETMUL)
               {
                if (!assign()) return qnan;
               }
              v_stack[v_sp-1].var = NULL;
              break;

            case toDIV:
            case toSETDIV:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
               {
                error(v_stack[v_sp-2].pos, "Illegal string operation");
                return qnan;
               }
              else
              if (v_stack[v_sp-1].get() == 0.0)
               {
                error(v_stack[v_sp-2].pos, "Division by zero");
                return qnan;
               }
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
               {
                v_stack[v_sp-2].ival /= v_stack[v_sp-1].ival;
               }
              else
               {
                if (v_stack[v_sp-1].tag == tvPERCENT)
                 {
                  float_t left = v_stack[v_sp-2].get();
                  float_t right = v_stack[v_sp-1].get();
                  v_stack[v_sp-2].fval = left/(left*right/100.0);
                 }
                else v_stack[v_sp-2].fval =
                   v_stack[v_sp-2].get() / v_stack[v_sp-1].get();
                v_stack[v_sp-2].tag = tvFLOAT;
               }
             v_sp -= 1;
             if (cop == toSETDIV)
              {
               if (!assign()) return qnan;
              }
             v_stack[v_sp-1].var = NULL;
            break;

            case toPAR:
             if ((v_stack[v_sp-1].tag == tvSTR) ||
                 (v_stack[v_sp-2].tag == tvSTR))
              {
               error(v_stack[v_sp-2].pos, "Illegal string operation");
               return qnan;
              }
             else
             if ((v_stack[v_sp-1].get() == 0.0) ||
                 (v_stack[v_sp-2].get() == 0.0))
              {
               error(v_stack[v_sp-2].pos, "Division by zero");
               return qnan;
              }
             if (v_stack[v_sp-1].tag == tvPERCENT)
              {
               float_t left = v_stack[v_sp-2].get();
               float_t right = v_stack[v_sp-1].get();
               v_stack[v_sp-2].fval = 1/(1/left+1/(left*right/100.0));
              }
             else v_stack[v_sp-2].fval = 1/(1/v_stack[v_sp-1].get()+1/v_stack[v_sp-2].get());
             v_stack[v_sp-2].tag = tvFLOAT;
             v_sp -= 1;
             v_stack[v_sp-1].var = NULL;
            break;

            case toPERCENT:
             if ((v_stack[v_sp-1].tag == tvSTR) ||
                 (v_stack[v_sp-2].tag == tvSTR))
               {
                 error(v_stack[v_sp-2].pos, "Illegal string operation");
                 return qnan;
               }
             else
             if ((v_stack[v_sp-1].get() == 0.0) ||
                 (v_stack[v_sp-2].get() == 0.0))
              {
               error(v_stack[v_sp-2].pos, "Division by zero");
               return qnan;
              }
             if (v_stack[v_sp-1].tag == tvPERCENT)
              {
               float_t left = v_stack[v_sp-2].get();
               float_t right = v_stack[v_sp-1].get();
               right = left*right/100.0;
               v_stack[v_sp-2].fval = 100.0*(left-right)/right;
              }
             else
              {
               float_t left = v_stack[v_sp-2].get();
               float_t right = v_stack[v_sp-1].get();
               v_stack[v_sp-2].fval = 100.0*(left-right)/right;
              }
             v_stack[v_sp-2].tag = tvFLOAT;
             v_sp -= 1;
             v_stack[v_sp-1].var = NULL;
            break;

            case toMOD:
            case toSETMOD:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
               {
                error(v_stack[v_sp-2].pos, "Illegal string operation");
                return qnan;
               }
              else
              if (v_stack[v_sp-1].get() == 0.0)
               {
                error(v_stack[v_sp-2].pos, "Division by zero");
                return qnan;
               }
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
               {
                v_stack[v_sp-2].ival %= v_stack[v_sp-1].ival;
               }
              else
               {
                if (v_stack[v_sp-1].tag == tvPERCENT)
                 {
                  float_t left = v_stack[v_sp-2].get();
                  float_t right = v_stack[v_sp-1].get();
                  v_stack[v_sp-2].fval = fmod(left, left*right/100.0);
                 }
                else v_stack[v_sp-2].fval =
                    fmod(v_stack[v_sp-2].get(), v_stack[v_sp-1].get());
                v_stack[v_sp-2].tag = tvFLOAT;
               }
              v_sp -= 1;
              if (cop == toSETMOD)
               {
                if (!assign()) return qnan;
               }
              v_stack[v_sp-1].var = NULL;
              break;

            case toPOW:
            case toSETPOW:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
               {
                v_stack[v_sp-2].ival =
                  (int_t)pow((float_t)v_stack[v_sp-2].ival,
                             (float_t)v_stack[v_sp-1].ival);
               }
              else
               {
                if (v_stack[v_sp-1].tag == tvPERCENT)
                 {
                  float_t left = v_stack[v_sp-2].get();
                  float_t right = v_stack[v_sp-1].get();
                  v_stack[v_sp-2].fval = pow(left, left*right/100.0);
                 }
                else v_stack[v_sp-2].fval =
                   pow(v_stack[v_sp-2].get(), v_stack[v_sp-1].get());
                 v_stack[v_sp-2].tag = tvFLOAT;
               }
              v_sp -= 1;
              if (cop == toSETPOW)
               {
                if (!assign()) return qnan;
               }
              v_stack[v_sp-1].var = NULL;
              break;

            case toAND:
            case toSETAND:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
               {
                error(v_stack[v_sp-2].pos, "Illegal string operation");
                return qnan;
               }
              else
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
               {
                v_stack[v_sp-2].ival &= v_stack[v_sp-1].ival;
               }
              else
               {
                v_stack[v_sp-2].ival =
                  v_stack[v_sp-2].get_int() & v_stack[v_sp-1].get_int();
                v_stack[v_sp-2].tag = tvINT;
               }
              v_sp -= 1;
              if (cop == toSETAND)
               {
                if (!assign()) return qnan;
               }
              v_stack[v_sp-1].var = NULL;
              break;

            case toOR:
            case toSETOR:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
                {
                  v_stack[v_sp-2].ival |= v_stack[v_sp-1].ival;
                }
              else
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].get_int() | v_stack[v_sp-1].get_int();
                  v_stack[v_sp-2].tag = tvINT;
                }
              v_sp -= 1;
              if (cop == toSETOR)
                {
                  if (!assign()) return qnan;
                }
              v_stack[v_sp-1].var = NULL;
              break;

            case toXOR:
            case toSETXOR:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
                {
                  v_stack[v_sp-2].ival ^= v_stack[v_sp-1].ival;
                }
              else
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].get_int() ^ v_stack[v_sp-1].get_int();
                  v_stack[v_sp-2].tag = tvINT;
                }
              v_sp -= 1;
              if (cop == toSETXOR)
                {
                  if (!assign()) return qnan;
                }
              v_stack[v_sp-1].var = NULL;
              break;

            case toASL:
            case toSETASL:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
                {
                  v_stack[v_sp-2].ival <<= v_stack[v_sp-1].ival;
                }
              else
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].get_int() << v_stack[v_sp-1].get_int();
                  v_stack[v_sp-2].tag = tvINT;
                }
              v_sp -= 1;
              if (cop == toSETASL)
                {
                  if (!assign()) return qnan;
                }
              v_stack[v_sp-1].var = NULL;
              break;

            case toASR:
            case toSETASR:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
                {
                  v_stack[v_sp-2].ival >>= v_stack[v_sp-1].ival;
                }
              else
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].get_int() >> v_stack[v_sp-1].get_int();
                  v_stack[v_sp-2].tag = tvINT;
                }
              v_sp -= 1;
              if (cop == toSETASR)
                {
                  if (!assign()) return qnan;
                }
              v_stack[v_sp-1].var = NULL;
              break;

            case toLSR:
            case toSETLSR:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
                {
                  v_stack[v_sp-2].ival =
                    (unsigned_t)v_stack[v_sp-2].ival >> v_stack[v_sp-1].ival;
                }
              else
                {
                  v_stack[v_sp-2].ival = (unsigned_t)v_stack[v_sp-2].get_int()
                                         >> v_stack[v_sp-1].get_int();
                  v_stack[v_sp-2].tag = tvINT;
                }
              v_sp -= 1;
              if (cop == toSETLSR)
                {
                  if (!assign()) return qnan;
                }
              v_stack[v_sp-1].var = NULL;
              break;

            case toEQ:
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].ival == v_stack[v_sp-1].ival;
                }
              else
              if (v_stack[v_sp-1].tag == tvSTR && v_stack[v_sp-2].tag == tvSTR)
                {
                  v_stack[v_sp-2].ival = (strcmp(v_stack[v_sp-2].sval, v_stack[v_sp-1].sval) == 0);
                  v_stack[v_sp-2].tag = tvINT;
                  free(v_stack[v_sp-2].sval);
                  v_stack[v_sp-2].sval = NULL;
                }
              else
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].get() == v_stack[v_sp-1].get();
                  v_stack[v_sp-2].tag = tvINT;
                }
              v_sp -= 1;
              v_stack[v_sp-1].var = NULL;
              break;

            case toNE:
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].ival != v_stack[v_sp-1].ival;
                }
              else
              if (v_stack[v_sp-1].tag == tvSTR && v_stack[v_sp-2].tag == tvSTR)
                {
                  v_stack[v_sp-2].ival = (strcmp(v_stack[v_sp-2].sval, v_stack[v_sp-1].sval) != 0);
                  v_stack[v_sp-2].tag = tvINT;
                  free(v_stack[v_sp-2].sval);
                  v_stack[v_sp-2].sval = NULL;
                }
              else
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].get() != v_stack[v_sp-1].get();
                  v_stack[v_sp-2].tag = tvINT;
                }
              v_sp -= 1;
              v_stack[v_sp-1].var = NULL;
              break;

            case toGT:
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].ival > v_stack[v_sp-1].ival;
                }
              else
              if (v_stack[v_sp-1].tag == tvSTR && v_stack[v_sp-2].tag == tvSTR)
                {
                  v_stack[v_sp-2].ival = (strcmp(v_stack[v_sp-2].sval, v_stack[v_sp-1].sval) > 0);
                  v_stack[v_sp-2].tag = tvINT;
                  free(v_stack[v_sp-2].sval);
                  v_stack[v_sp-2].sval = NULL;
                }
              else
                {
                  v_stack[v_sp-2].ival =
                      v_stack[v_sp-2].get() > v_stack[v_sp-1].get();
                  v_stack[v_sp-2].tag = tvINT;
                }
              v_sp -= 1;
              v_stack[v_sp-1].var = NULL;
              break;

            case toGE:
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].ival >= v_stack[v_sp-1].ival;
                }
              else
              if (v_stack[v_sp-1].tag == tvSTR && v_stack[v_sp-2].tag == tvSTR)
                {
                  v_stack[v_sp-2].ival = (strcmp(v_stack[v_sp-2].sval, v_stack[v_sp-1].sval) >= 0);
                  v_stack[v_sp-2].tag = tvINT;
                  free(v_stack[v_sp-2].sval);
                  v_stack[v_sp-2].sval = NULL;
                }
              else
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].get() >= v_stack[v_sp-1].get();
                  v_stack[v_sp-2].tag = tvINT;
                }
              v_sp -= 1;
              v_stack[v_sp-1].var = NULL;
              break;

            case toLT:
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].ival < v_stack[v_sp-1].ival;
                }
              else
              if (v_stack[v_sp-1].tag == tvSTR && v_stack[v_sp-2].tag == tvSTR)
                {
                  v_stack[v_sp-2].ival = (strcmp(v_stack[v_sp-2].sval, v_stack[v_sp-1].sval) < 0);
                  v_stack[v_sp-2].tag = tvINT;
                  free(v_stack[v_sp-2].sval);
                  v_stack[v_sp-2].sval = NULL;
                }
              else
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].get() < v_stack[v_sp-1].get();
                  v_stack[v_sp-2].tag = tvINT;
                }
              v_sp -= 1;
              v_stack[v_sp-1].var = NULL;
              break;

            case toLE:
              if (v_stack[v_sp-1].tag == tvINT && v_stack[v_sp-2].tag == tvINT)
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].ival <= v_stack[v_sp-1].ival;
                }
              else
              if (v_stack[v_sp-1].tag == tvSTR && v_stack[v_sp-2].tag == tvSTR)
                {
                  v_stack[v_sp-2].ival = (strcmp(v_stack[v_sp-2].sval, v_stack[v_sp-1].sval) <= 0);
                  v_stack[v_sp-2].tag = tvINT;
                  free(v_stack[v_sp-2].sval);
                  v_stack[v_sp-2].sval = NULL;
                }
              else
                {
                  v_stack[v_sp-2].ival =
                    v_stack[v_sp-2].get() <= v_stack[v_sp-1].get();
                  v_stack[v_sp-2].tag = tvINT;
                }
              v_sp -= 1;
              v_stack[v_sp-1].var = NULL;
              break;

            case toPREINC:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].tag == tvINT)
                {
                  v_stack[v_sp-1].ival += 1;
                }
              else
                {
                  v_stack[v_sp-1].fval += 1;
                }
              if (!assign()) return qnan;
              v_stack[v_sp-1].var = NULL;
              break;

            case toPREDEC:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].tag == tvINT)
                {
                  v_stack[v_sp-1].ival -= 1;
                }
              else
                {
                  v_stack[v_sp-1].fval -= 1;
                }
              if (!assign()) return qnan;
              v_stack[v_sp-1].var = NULL;
              break;

            case toPOSTINC:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].var == NULL)
                {
                  error(v_stack[v_sp-1].pos, "Varaibale expected");
                  return qnan;
                }
              if (v_stack[v_sp-1].var->val.tag == tvINT)
                {
                  v_stack[v_sp-1].var->val.ival += 1;
                }
              else
                {
                  v_stack[v_sp-1].var->val.fval += 1;
                }
              v_stack[v_sp-1].var = NULL;
              break;

            case toPOSTDEC:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].var == NULL)
                {
                  error(v_stack[v_sp-1].pos, "Varaibale expected");
                  return qnan;
                }
              if (v_stack[v_sp-1].var->val.tag == tvINT)
                {
                  v_stack[v_sp-1].var->val.ival -= 1;
                }
              else
                {
                  v_stack[v_sp-1].var->val.fval -= 1;
                }
              v_stack[v_sp-1].var = NULL;
              break;

            case toFACT:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].tag == tvINT)
                {
                  v_stack[v_sp-1].ival = (int_t)Factorial((float_t)v_stack[v_sp-1].ival);
                }
              else
                {
                  v_stack[v_sp-1].fval = (float_t)Factorial((float_t)v_stack[v_sp-1].fval);
                }
              v_stack[v_sp-1].var = NULL;

            break;

            case toSET:
              if ((v_sp < 2) || (v_stack[v_sp-2].var == NULL))
                {
                  if (v_sp < 2) error("Variabale expected");
                  else error(v_stack[v_sp-2].pos, "Variabale expected");
                  return qnan;
                }
              else
                {
                  v_stack[v_sp-2]=v_stack[v_sp-2].var->val=v_stack[v_sp-1];
                }
              v_sp -= 1;
              v_stack[v_sp-1].var = NULL;
              break;

            case toNOT:
              if ((v_stack[v_sp-1].tag == tvSTR) ||
                  (v_stack[v_sp-2].tag == tvSTR))
                {
                  error(v_stack[v_sp-2].pos, "Illegal string operation");
                  return qnan;
                }
              else
              if (v_stack[v_sp-1].tag == tvINT)
                {
                  v_stack[v_sp-1].ival = !v_stack[v_sp-1].ival;
                }
              else
                {
                  v_stack[v_sp-1].ival = !v_stack[v_sp-1].fval;
                  v_stack[v_sp-1].tag = tvINT;
                }
              v_stack[v_sp-1].var = NULL;
              break;

            case toMINUS:
              if (v_stack[v_sp-1].tag == tvINT)
                {
                  v_stack[v_sp-1].ival = -v_stack[v_sp-1].ival;
                }
              else
                {
                  v_stack[v_sp-1].fval = -v_stack[v_sp-1].fval;
                }
              // no break

            case toPLUS:
              v_stack[v_sp-1].var = NULL;
              break;

            case toCOM:
              if (v_stack[v_sp-1].tag == tvINT)
                {
                  v_stack[v_sp-1].ival = ~v_stack[v_sp-1].ival;
                }
              else
                {
                  v_stack[v_sp-1].ival = ~(int)v_stack[v_sp-1].fval;
                  v_stack[v_sp-1].tag = tvINT;
                }
              v_stack[v_sp-1].var = NULL;
              break;

            case toRPAR:
              error("mismatched ')'");
              return qnan;

            case toFUNC:
              error("'(' expected");
              return qnan;

            case toLPAR:
              if (oper != toRPAR)
                {
                  error("')' expected");
                  return qnan;
                }

              if (o_stack[o_sp-1] == toFUNC)
                {
                  symbol* sym = v_stack[v_sp-n_args-1].var;
                  if (sym)
                  {
                  switch (sym->tag)
                    {
                    case tsIFUNC1:
                      if (n_args != 1)
                        {
                          error(v_stack[v_sp-n_args-1].pos,
                                "Function should take one argument");
                          return qnan;
                        }
                      v_stack[v_sp-2].ival =
                        (*(int_t(*)(int_t))sym->func)(v_stack[v_sp-1].get_int());
                      v_stack[v_sp-2].tag = tvINT;
                      v_sp -= 1;
                      break;

                    case tsIFUNC2:
                      if (n_args != 2)
                        {
                          error(v_stack[v_sp-n_args-1].pos,
                                "Function should take two arguments");
                          return qnan;
                        }
                      v_stack[v_sp-3].ival =
                        (*(int_t(*)(int_t,int_t))sym->func)
                        (v_stack[v_sp-2].get_int(), v_stack[v_sp-1].get_int());
                      v_stack[v_sp-3].tag = tvINT;
                      v_sp -= 2;
                      break;

                    case tsIFFUNC3:
                      if (n_args != 3)
                        {
                          error(v_stack[v_sp-n_args-1].pos,
                                "Function should take three arguments");
                          return qnan;
                        }
                      v_stack[v_sp-4].ival =
                        (*(int_t(*)(double, double, int_t))sym->func)
                        (v_stack[v_sp-3].get_dbl(), v_stack[v_sp-2].get_dbl(), v_stack[v_sp-1].get_int());
                      v_stack[v_sp-4].tag = tvINT;
                      v_sp -= 3;
                    break;

                    case tsFFUNC1:
                      if (n_args != 1)
                        {
                          error(v_stack[v_sp-n_args-1].pos,
                                "Function should take one argument");
                          return qnan;
                        }
                      v_stack[v_sp-2].fval =
                        (*(float_t(*)(float_t))sym->func)(v_stack[v_sp-1].get());
                      v_stack[v_sp-2].tag = tvFLOAT;
                      v_sp -= 1;
                      break;

                    case tsFFUNC2:
                      if (n_args != 2)
                        {
                          error(v_stack[v_sp-n_args-1].pos,
                                "Function should take two arguments");
                          return qnan;
                        }
                      v_stack[v_sp-3].fval =
                        (*(float_t(*)(float_t, float_t))sym->func)
                        (v_stack[v_sp-2].get(), v_stack[v_sp-1].get());
                      v_stack[v_sp-3].tag = tvFLOAT;
                      v_sp -= 2;
                      break;

                    case tsFFUNC3:
                      if (n_args != 3)
                        {
                          error(v_stack[v_sp-n_args-1].pos,
                                "Function should take three arguments");
                          return qnan;
                        }
                      v_stack[v_sp-4].fval =
                        (*(float_t(*)(float_t, float_t, float_t))sym->func)
                        (v_stack[v_sp-3].get(), v_stack[v_sp-2].get(), v_stack[v_sp-1].get());
                      v_stack[v_sp-4].tag = tvFLOAT;
                      v_sp -= 3;
                    break;

                    case tsPFUNCn:
                      if (n_args < 1)
                        {
                          error(v_stack[v_sp-n_args-1].pos,
                                "Function should take one or more arguments");
                          return qnan;
                        }
                      v_stack[v_sp-n_args-1].tag = tvSTR;
                      v_stack[v_sp-n_args-1].ival = 0;
                      v_stack[v_sp-n_args-1].sval = (char *)malloc(STRBUF);

                      (*(int_t(*)(char *,char *, int, value*))sym->func)
                       (v_stack[v_sp-n_args-1].sval,
                        v_stack[v_sp-n_args].get_str(),
                        n_args-1, &v_stack[v_sp-n_args+1]);
                      strcpy(sres, v_stack[v_sp-n_args-1].sval);
                      if (n_args > 1)
                       {
                        #ifdef _long_double_
                        //DBL_MAX
                        v_stack[v_sp-n_args-1].ival = v_stack[v_sp-n_args+1].ival;
                        if (v_stack[v_sp-n_args+1].fval > maxdbl) v_stack[v_sp-n_args-1].fval = qnan;
                        else v_stack[v_sp-n_args-1].fval = v_stack[v_sp-n_args+1].fval;
                        //v_stack[v_sp-n_args-1].fval = v_stack[v_sp-n_args+1].fval;
                        #else /*_long_double_*/
                        v_stack[v_sp-n_args-1].ival = v_stack[v_sp-n_args+1].ival;
                        //v_stack[v_sp-n_args-1].fval = v_stack[v_sp-n_args+1].fval;
                        #endif /*_long_double_*/
                       }
                      v_sp -= n_args;
                    break;
                    case tsSIFUNC1:
                      if (n_args != 1)
                        {
                          error(v_stack[v_sp-n_args-1].pos,
                                "Function should take one argument");
                          return qnan;
                        }
                      v_stack[v_sp-2].ival =
                        (*(int_t(*)(char *))sym->func)(v_stack[v_sp-1].get_str());
                      v_stack[v_sp-2].tag = tvINT;
                      v_sp -= 1;
                    break;
                    default:
                      error("Invalid expression");
                    }
                  }
                  v_stack[v_sp-1].var = NULL;
                  o_sp -= 1;
                  n_args = 1;
                }
              else
              if (n_args != 1)
                {
                  error("Function call expected");
                  return qnan;
                }
              goto next_token;
            default:
              error("syntax error");
           }
        }
      if (o_sp == max_stack_size)
        {
          error("operator stack overflow");
          return qnan;
        }
      o_stack[o_sp++] = oper;
    }
}

