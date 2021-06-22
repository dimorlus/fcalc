//---------------------------------------------------------------------------
#if defined(_WIN64)||defined(_WIN32)
#define _WIN_
#include <windows.h>
#endif
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#pragma hdrstop
#include "scalc.h"
#include "sfmts.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma warn -8008 // Condition is always true
#pragma warn -8066 // Unreachable code

int _matherr(struct _exception *e)
{
  return 1;             /* error has been handled */
}

//----------------------------------
int ones(unsigned char *cp, int from, int to)
{
 int res = 0;
 for(int i = from; i < to; i++)
   if (cp[i/8] & (1 << (i%8))) res++;
 return res;
}
//---------------------------------------------------------------------------

bool isNan(float_t d)
{
 int fr, ex;
 if (d < 0) d = -d;
 if (sizeof(d) == 80/8) //IEEE 754 80 bits
  {
   fr = ones((unsigned char *)&d, 0, 64);
   ex = ones((unsigned char *)&d, 64, 80);
   return ((ex >= 15) && fr);
  }
 else
 if (sizeof(d) == 64/8) //IEEE 754 64 bits
  {
   fr = ones((unsigned char *)&d, 0, 52);
   ex = ones((unsigned char *)&d, 52, 64);
   return ((ex >= 11) && fr);
  }
 else
 if (sizeof(d) == 32/8) //IEEE 754 32 bits
  {
   fr = ones((unsigned char *)&d, 0, 23);
   ex = ones((unsigned char *)&d, 23, 32);
   return ((ex >= 8) && fr);
  }
 return false;
}
//---------------------------------------------------------------------------

const unsigned __int64 dms[] =
   {(60i64*60*60*24*365.25*100i64),(60i64*60*24*365.25),
    (60i64*60*24), (60i64*60), 60i64, 1i64};

int t2str(char *str, __int64 sec)
{
 const char * fmt[] =
   {"%d:c ", "%d:y ", "%d:d ", "%d:h ", "%d:m ", "%d:s "};
 unsigned int pt[6];
 int i, j, k;
 char *pc = str;

 for(i = 0, j = -1, k = 0; i < 6; i++)
  {
    pt[i] = (unsigned int)(sec / dms[i]);
    sec %= dms[i];
    if ((j == -1) && (pt[i] != 0)) j = i;
    if ((j != -1) && (pt[i] != 0)) k = i;
  }
 *str = '\0';
 if (j == -1) str += sprintf(str, "0s");
 else
 for(i = j; i <= k; i++)
   str += sprintf(str, fmt[i], pt[i]);
 return str-pc;
}
//---------------------------------------------------------------------------

/* convert int to bin according format %10b */
int b2str(char *str, const char *fmt, unsigned __int64 b)
{
  char c;
  bool flag = false;
  int j = 0;
  char *ws;
  int wide;
  int i,w;
  unsigned __int64 mask;

  while(0 != (c = *fmt++))
   {
    if (!flag && (c == '%')) {flag = true; ws = (char*)fmt; continue;}
    if (flag && (c == '%')) {flag = false; continue;}
    if (flag && (c == 'b'))
     {
      for(wide = 0; ws < fmt; ws++)
       {
        c = *ws;
        if (c == '-') continue;
        if ((c >= '0') && (c <= '9')) wide = wide * 10 + (c - '0');
        else break;
       }

      mask = 1i64 << 63;
      for(w = 64; ((w > 0) && (mask > 1) && ((b & mask) == 0)); w--) mask >>=1;
      if (wide <= w) wide = w;

      mask = 1i64 << (wide-1);
      for(i = 0; i < wide; i++)
       {
        if (b & mask) str[j++] = '1';
        else str[j++] = '0';
        mask >>=1;
       }
      flag = false;
      continue;
     }
    if (!flag) str[j++] = c;
   }
  str[j] = '\0';
  return j;
}
//---------------------------------------------------------------------------

int d2scistr(char *str, float_t d)
{
 const char csci[] = {'y', 'z', 'a', 'f', 'p', 'n', 'u', 'm', ' ',
                      'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'};
 enum {yocto, zepto, atto, femto, pico, nano, micro, milli, empty,
      kilo, mega, giga, tera, peta, exa, zetta, yotta};

 if (isNan(d)) return sprintf(str, "%e", d);
 else
  {
   #ifdef _long_double_
   float_t dd = fabsl(d);
   #else /*_long_double_*/
   float_t dd = fabs(d);
   #endif /*_long_double_*/
   int rng = empty;
   if (dd > 0)
    {
     while (dd >= 1000)
      {
       rng++;
       dd /= 1000;
       if (rng > yotta) break;
      }
     while (dd < 1)
      {
       rng--;
       dd *= 1000;
       if (rng < yocto) break;
      }
    }
   if (d < 0) dd = -dd;
   #ifdef _long_double_
   if (rng == empty)
    return sprintf(str, "%.4Lg", d);
   else
   if ((rng >= yocto) && (rng <= yotta))
    return sprintf(str, "%.4Lg%c", dd, csci[rng]);
   else
    return sprintf(str, "%.2Le", d);
   #else /*_long_double_*/
   if (rng == empty)
    return sprintf(str, "%.4g", d);
   else
   if ((rng >= yocto) && (rng <= yotta))
    return sprintf(str, "%.4g%c", dd, csci[rng]);
   else
    return sprintf(str, "%.2e", d);
   #endif /*_long_double_*/
  }
}
//---------------------------------------------------------------------------

int d2nrmstr(char *str, float_t d)
{
 if (isNan(d)) return sprintf(str, "%e", d);
 else
  {
   #ifdef _long_double_
   float_t dd = fabsl(d);
   #else /*_long_double_*/
   float_t dd = fabs(d);
   #endif /*_long_double_*/
   int rng = 0;
   if (dd > 0)
    {
     while (dd >= 1000)
      {
       rng++;
       dd /= 1000;
      }
     while (dd < 1)
      {
       rng--;
       dd *= 1000;
      }
    }
   if (d < 0) dd = -dd;
   #ifdef _long_double_
   if (rng == 0) return sprintf(str, "%.4Lg", d);
   else return sprintf(str, "%.4Lge%i", dd, rng*3);
   #else /*_long_double_*/
   if (rng == 0) return sprintf(str, "%.4g", d);
   else return sprintf(str, "%.4ge%i", dd, rng*3);
   #endif /*_long_double_*/
  }
}
//---------------------------------------------------------------------------

int b2scistr(char *str, float_t d)
{
 const char csci_plus[] =  {' ', 'K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'};
 enum {Empty, Kilo, Mega, Giga, Tera, Peta, Exa, Zetta, Yotta};

 if (isNan(d)) return sprintf(str, "%e", d);
 else
  {
   #ifdef _long_double_
   float_t dd = fabsl(d);
   #else /*_long_double_*/
   float_t dd = fabs(d);
   #endif /*_long_double_*/
   int rng = Empty;
   if (dd > 0)
    {
     while (dd >= 1024)
      {
       rng++;
       dd /= 1024;
      }
    }
   if (d < 0) dd = -dd;

   #ifdef _long_double_
   if ((rng > Empty) && (rng <= Yotta))
    return sprintf(str, "%.4Lg%cB", dd, csci_plus[rng]);
   else
    return sprintf(str, "%.4Lg", d);
   #else /*_long_double_*/
   if ((rng > Empty) && (rng <= Yotta))
    return sprintf(str, "%.4g%cB", dd, csci_plus[rng]);
   else
    return sprintf(str, "%.4g", d);
   #endif /*_long_double_*/
  }
}
//---------------------------------------------------------------------------

int dgr2str(char *str, float_t d)
{
 const char cdeg[] = {96, 39, 34}; //` - degrees, ' - minutes, " - seconds
 const float_t mdeg[] = {M_PI/180.0, M_PI/(180.0*60), M_PI/(180.0*60*60)};
 unsigned int pt[6];
 int i, j, k;
 char *pc = str;

 for(i = 0, j = -1, k = 0; i < 3; i++)
  {
    pt[i] = (unsigned int)(d / mdeg[i]);
    d = fmod(d, mdeg[i]);
    if ((j == -1) && (pt[i] != 0)) j = i;
    if ((j != -1) && (pt[i] != 0)) k = i;
  }
 *str = '\0';
 if (j == -1) str += sprintf(str, "0%c",cdeg[2]);
 else
 for(i = j; i <= k; i++)
   str += sprintf(str, "%d%c", pt[i], cdeg[i]);
 return str-pc;
}
//---------------------------------------------------------------------------

int chr2str(char *str, unsigned char c)
{
 const char* const ch[] =
 // 0        1        2        3        4        5        6        7
  {"NUL",   "SOH",   "STX",   "ETX",   "EOT",   "ENQ",   "ACK",   "BEL",  //0
   "BS",    "HT",    "LF",    "VT",    "FF",    "CR",    "SO",    "SI",   //1
   "DLE",   "DC1",   "DC2",   "DC3",   "DC4",   "NAK",   "SYN",   "ETB",  //2
   "CAN",   "EM",    "SUB",   "ESC",   "FS",    "GS",    "RS",    "US"};  //3

 const char* const chc[] =
 // 0        1        2        3        4        5        6       7
  {"\\000", "\\001", "\\002", "\\003", "\\004", "\\005", "\\006", "\\a",    //0
   "\\b",   "\\t",   "\\n",   "\\v",   "\\004", "\\r",   "\\016", "\\017",  //1
   "\\020", "\\021", "\\022", "\\023", "\\024", "\\025", "\\026", "\\027",  //2
   "\\030", "\\031", "\\032", "\\e",   "\\034", "\\035", "\\036", "\\037"}; //3

 if (c < 32) return sprintf(str, "%s '%s'", ch[c], chc[c]);
 else
 if (c == '\\') return sprintf(str, "'\\' '%c'", c);
 else
 if (c == 255U) return sprintf(str, "'\\f' '%c'", c);
 else return sprintf(str, "'%c'", c);
}
//---------------------------------------------------------------------------

int wchr2str(char *str, int i)
{
#ifdef _WIN_
  wchar_t wbuf[2];
  char cbuf[8];

  wbuf[0] = *(wchar_t *)&i;
  wbuf[1] = L'\0';


  WideCharToMultiByte(CP_OEMCP, 0, wbuf, -1,
               (LPSTR)cbuf, 4, NULL, NULL);
  return sprintf(str, "'%s'W", cbuf);
#else /*_WIN_*/
 *str = '\0';
 return 0;
#endif  /*_WIN_*/
}
//---------------------------------------------------------------------------

//   https://support.microsoft.com/en-us/help/167296/how-to-convert-a-unix-time-t-to-a-win32-filetime-or-systemtime
//   #include <winbase.h>
//   #include <winnt.h>
//   #include <time.h>
//
//   void UnixTimeToFileTime(time_t t, LPFILETIME pft)
//   {
//     // Note that LONGLONG is a 64-bit value
//     LONGLONG ll;
//
//     ll = Int32x32To64(t, 10000000) + 116444736000000000;
//     pft->dwLowDateTime = (DWORD)ll;
//     pft->dwHighDateTime = ll >> 32;
//   }

int nx_time2str(char *str, __int64 time)
{
 struct tm t = *gmtime((time_t*)&time);
 return strftime(str, 80, "%a, %b %d %H:%M:%S %Y", &t);
 //return sprintf(str, "%s", ctime((time_t*)&time));
}
//---------------------------------------------------------------------------


void fraction(double val, double eps, int &num, int &denum)
{
  int a = 1, b = 1;
  int mn = 2;

  if (val < eps)
   {
    num = denum = 0;
    return;
   }

  double c = 1;
  do
   {
    b++;
    c = (double)a / b;
   }
  while ((val - c) < 0);

  if ((val - c) < eps)
   {
    num = a; denum = b;
    return;
   }

  b--;
  c = (double)a / b;
  if ((val - c) > -eps)
   {
    num = a; denum = b;
    return;
   }

  for(int iter = 0; iter < 20000; iter++)
   {
    int cc = a*mn, zz = b*mn;
    do
     {
      zz++;
      c = (double)cc / zz;
     }
    while ((val - c) < 0);
    if ((val - c) < eps)
     {
      num = cc; denum = zz;
      return;
     }
    zz--;
    c = (double)cc / zz;
    if ((val - c) > -eps)
     {
      num = cc; denum = zz;
      return;
     }
    mn++;
   }
}
//---------------------------------------------------------------------------

int d2frcstr(char *str, float_t d, int eps_order)
{
 int num, denum;
 double val;
 double eps;
 if ((eps_order > 0) && (eps_order < 7)) eps = exp(-eps_order*log(10.0));
 else eps = 0.001;
 if (d > 0) val = d;
 else val = -d;
 double intpart = floor(val);
 if (intpart > 0)
  {
   fraction(d-intpart, eps, num, denum);
   if (denum)
    {
     if (d > 0) return sprintf(str, "%.0f+%d/%d", intpart, num, denum);
     else return sprintf(str, "-%.0f-%d/%d", intpart, num, denum);
    }
   else return sprintf(str, "%f", d);
  }
 else
  {
   fraction(val, eps, num, denum);
   if (denum)
    {
     if (d > 0) return sprintf(str, "%d/%d", num, denum);
     else return sprintf(str, "-%d/%d", num, denum);
    }
   else return sprintf(str, "%f", d);
  }
}
//---------------------------------------------------------------------------

