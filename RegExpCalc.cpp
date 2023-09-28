//---------------------------------------------------------------------------
#define _INI_
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <IniFiles.hpp>
#include <Registry.hpp>
#include <HtmlHelp.h>
#pragma hdrstop

#include "RegExpClass.h"
#include "StrUtils.h"
#include "scalc.h"
#include "HTMLHelpViewer.hpp"
#include "RegExpCalc.h"

#define BUFSIZE         4096
#define ARGSIZE         128
#pragma warn -8004
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#pragma link "HTMLHelpViewer"
TRegExpCalcFrm *RegExpCalcFrm;
//---------------------------------------------------------------------------
__fastcall TRegExpCalcFrm::TRegExpCalcFrm(TComponent* Owner)
        : TForm(Owner)
{
 HelpFile = ExtractFilePath(Application->ExeName) + "RegExpCalc.chm";
 for(int i = 0; i < _RLINES_; i++)
  {
   for(int j = 0; j < _RCOLUMNS_; j++)
    {
     LB[j][i] = new TLabel(this);
     LB[j][i]->Parent = this;
    }
  }
}
//---------------------------------------------------------------------------
//https://bbs.archlinux.org/viewtopic.php?id=31087
//scanf
//int main() {
//  char* m = (char*) malloc(sizeof(int)*2 + sizeof(char*)); /* prepare enough memory*/
//  void* bm = m; /* copies the pointer */
//  char* string = "I am a string!!"; /* an example string */
//
//  (*(int*)m) = 10; /*puts the first value */
//  m += sizeof(int); /* move forward the pointer to the next element */
//
//  (*(char**)m) = string; /* puts the next value */
//  m += sizeof(char*); /* move forward again*/
//
//  (*(int*)m) = 20; /* puts the third element */
//  m += sizeof(int); /* unneeded, but here for clarity. */
//
//  vprintf("%d %s %d\n", bm); /* the deep magic starts here...*/
//  free(bm);
//}
//---------------------------------------------------------------------------

int ssctst(char *dst, const char *sfmt, const char *str)
{
 char c = '\0';
 char cc = '\0';
 int ii = 0;
 int n = 0;
 int pfsize = 0;
 bool percent, open;
 enum ftypes {tNone, tChar, tShort, tInt, tLong, ti64,
              tFloat, tDouble, tExt, tString} fmt;
 void *arglist[ARGSIZE];
 char typelist[ARGSIZE];
 char fmtlist[ARGSIZE];
 memset(arglist, 0, sizeof(arglist));
 memset(typelist, 0, sizeof(typelist));
 memset(fmtlist, 0, sizeof(typelist));
 do
  {
   open = false;
   percent = false;
   fmt = tNone;
   do
    {
     cc = c;
     c = sfmt[ii++];
     if (open)
      {
       if ((c == ']'))
        {
         fmt = tString;
         c = 's';
         open = false;
         break;
        }
       else continue;
      }
     if (percent)
      {
       if (c == '%')
        {
         percent = false;
         continue;
        }
       else
       if (c == '*')
        {
         fmt = tNone;
         percent = false;
         continue;
        }
       else
       if ((c == 'f') || (c == 'e') || (c == 'E') ||
           (c == 'g') || (c == 'G'))
        {
         fmt = tFloat;
         if (cc == 'l') fmt = tDouble;
         else
         if (cc == 'L') fmt = tExt;
         break;
        }
       else
       if ((c == 'd') || (c == 'i') || (c == 'u') ||
           (c == 'x') || (c == 'X') || (c == 'o'))
        {
         fmt = tInt;
         if (cc == 'L') fmt = ti64;
         else
         if (cc == 'l') fmt = tLong;
         else
         if (cc == 'h') fmt = tShort;
         break;
        }
       else
       if ((c == 'D') || (c == 'I') || (c == 'U') ||
           (c == 'O'))
        {
         fmt = tLong;
         c = tolower(c);
         break;
        }
       else
       if (c == 'c')
        {
         //fmt = tChar;
         fmt = tString;
         break;
        }
       if (c == 's')
        {
         fmt = tString;
         break;
        }
       else
       if (c == '[')
        {
         open = true;
         continue;
        }
      }
     else
     if (c == '%')
      {
       percent = true;
       continue;
      }
    }
   while (c);
   if (c && percent && fmt)
    {
     typelist[n] = fmt;
     fmtlist[n] = c;
     switch(fmt)
      {
       case tChar:
        arglist[n] = (void *)malloc(sizeof(char));
        *(char *)arglist[n] = '\0';
        pfsize += sizeof(char);
       break;
       case tShort:
        arglist[n] = (void *)malloc(sizeof(short));
        *(short *)arglist[n] = 0;
        pfsize += sizeof(short);
       break;
       case tInt:
        arglist[n] = (void *)malloc(sizeof(int));
        *(int *)arglist[n] = 0;
        pfsize += sizeof(int);
       break;
       case tLong:
        arglist[n] = (void *)malloc(sizeof(long));
        *(long *)arglist[n] = 0;
        pfsize += sizeof(long);
       break;
       case ti64:
        arglist[n] = (void *)malloc(sizeof(__int64));
        *(__int64 *)arglist[n] = 0;
        pfsize += sizeof(__int64);
       break;
       case tFloat:
        arglist[n] = (void *)malloc(sizeof(float));
        *(float *)arglist[n] = 0.0;
        pfsize += sizeof(float);
       break;
       case tDouble:
        arglist[n] = (void *)malloc(sizeof(double));
        *(double *)arglist[n] = 0.0;
        pfsize += sizeof(double);
       break;
       case tExt:
        arglist[n] = (void *)malloc(sizeof(long double));
        *(long double *)arglist[n] = 0.0;
        pfsize += sizeof(long double);
       break;
       case tString:
        arglist[n] = (void *)malloc(sizeof(char *));
        *(char**)arglist[n] = (char *)malloc(BUFSIZE);
        memset(*(char**)arglist[n], 0, BUFSIZE);
        pfsize += sizeof(char *);
       break;
      }
     n++;
    }
  }
 while (c && (n < BUFSIZE));

 arglist[n] = (void *)malloc(sizeof(long));

 int nn = vsscanf(str, sfmt, arglist);

 char *fdest = dst;
 for(int i=0; (i < n) && arglist[i]; i++)
  {
   switch(typelist[i])
    {
     case tChar:
      {
       char ci = *(char *)arglist[i];
       char fmt[5] = {"%c, \0"};
       //fmt[1] = fmtlist[i];
       fdest += sprintf(fdest, fmt, ci);
      }
     break;
     case tShort:
      {
       short si = *(short *)arglist[i];
       char fmt[6] = {"%hd, \0"};
       fmt[2] = fmtlist[i];
       fdest += sprintf(fdest, fmt, si);
      }
     break;
     case tInt:
      {
       int ii = *(int *)arglist[i];
       char fmt[5] = {"%d, \0"};
       fmt[1] = fmtlist[i];
       fdest += sprintf(fdest, fmt, ii);
      }
     break;
     case tLong:
      {
       long li = *(long *)arglist[i];
       char fmt[6] = {"%ld, \0"};
       fmt[2] = fmtlist[i];
       fdest += sprintf(fdest, fmt, li);
      }
     break;
     case ti64:
      {
       __int64 Li = *(__int64 *)arglist[i];
       char fmt[6] = {"%Ld, \0"};
       fmt[2] = fmtlist[i];
       fdest += sprintf(fdest, fmt, Li);
      }
     break;
     case tFloat:
      {
       float ff = *(float *)arglist[i];
       char fmt[5] = {"%f, \0"};
       fmt[1] = fmtlist[i];
       fdest += sprintf(fdest, fmt, ff);
      }
     break;
     case tDouble:
      {
       double df = *(double *)arglist[i];
       char fmt[6] = {"%lf, \0"};
       fmt[2] = fmtlist[i];
       fdest += sprintf(fdest, fmt, df);
      }
     break;
     case tExt:
      {
       long double lf = *(long double *)arglist[i];
       char fmt[6] = {"%Lf, \0"};
       fmt[2] = fmtlist[i];
       fdest += sprintf(fdest, fmt, lf);
      }
     break;
     case tString:
      {
       char *ss = (char*)arglist[i];
       char fmt[5] = {"%s, \0"};
       //fmt[1] = fmtlist[i];
       fdest += sprintf(fdest, fmt, ss);
      }
     break;
    }
  }
 if (fdest > dst+2) fdest[-2] = '\0'; //supress last ", "

 for(int i=0; (i<ARGSIZE) && arglist[i]; i++)
  {
   if (typelist[i] == tString)
    {
     char *s = (char*)arglist[i];
     free(s);
     free(arglist[i]);
    }
   else free(arglist[i]);
  }
 return nn;
}
//---------------------------------------------------------------------------
void sprtst(char *dst, char *sfmt, char *str, calculator *ccalc)
{
 char pfmt[BUFSIZE];
 char pstr[BUFSIZE];
 enum ftypes {tNone, tShort, tInt, tLong, tInt64,
  tFloat, tDouble, tLongDouble, tString, tPtr} fmt;
 char c, cc = '\0';
 int i;
 bool flag;

  do
   {
    i = 0; flag = false; fmt = tNone;
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
         {
          if (cc == 'l') fmt = tDouble;
          else
          if (cc == 'L') fmt = tLongDouble;
          else fmt = tFloat;
          break;
         }
        else
        if ((c == 'd') || (c == 'i') || (c == 'u') ||
            (c == 'x') || (c == 'X') || (c == 'o') || (c == 'c'))
         {
          if (cc == 'l') fmt = tLong;
          else
          if (cc == 'h') fmt = tShort;
          else
          if (cc == 'L') fmt = tInt64;
          else fmt = tInt;
          break;
         }
        else
        if ((c == 'n')||(c == 'p')) {fmt = tPtr; break;}
        else
        if (c == 's') {fmt = tString; break;}
        else continue;
       }
      else
      if (c == '%') {flag = true; continue;}
     }
    while (c && (i < BUFSIZE));
    if (!c) sfmt--; //poit to '\0'
    i = 0; flag = false;
    do
     {
      c = *str++;
      if (!flag && (c == '"')) {flag = true; continue;}
      if (flag && (c == '"')) {flag = false; continue;}
      if (!flag && ((c == ',') || (c == '\0'))) //add asterix
       {
        i = pstr[i] = '\0';
        switch(fmt)
         {
          case tNone:
           dst += sprintf(dst, pfmt);
          break;
          case tPtr:
           strcpy(dst, pfmt);
           dst += strlen(pfmt);
          break;
          case tInt:
           {
            int l = atol(pstr);
            dst += sprintf(dst, pfmt, l);
           }
          break;
          case tLong:
           {
            long l = atol(pstr);
            dst += sprintf(dst, pfmt, l);
           }
          break;
          case tInt64:
           {
            __int64 l = _atoi64(pstr);
            dst += sprintf(dst, pfmt, l);
           }
          break;
          case tShort:
           {
            short l = atol(pstr);
            dst += sprintf(dst, pfmt, l);
           }
          break;
          case tFloat:
           {
            __int64 iVal;
            float d;

            if (ccalc) d = ccalc->evaluate(pstr, &iVal);
            else d = (float)atof(pstr);

            dst += sprintf(dst, pfmt, d);
           }
          break;
          case tDouble:
           {
            __int64 iVal;
            double d;

            if (ccalc) d = ccalc->evaluate(pstr, &iVal);
            else d = (double)atof(pstr);

            dst += sprintf(dst, pfmt, d);
           }
          break;
          case tLongDouble:
           {
            __int64 iVal;
            long double d;

            if (ccalc) d = ccalc->evaluate(pstr, &iVal);
            else d = (long double)atof(pstr);

            dst += sprintf(dst, pfmt, d);
           }
          break;
          case tString:
           dst += sprintf(dst, pfmt, pstr);
          break;
         }
        break;
       }
      else pstr[i++] = c;
     }
    while (c  && (i < BUFSIZE));
   }
  while (*sfmt && (i < BUFSIZE));
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
const char * const ttls[] =
 {"Res:", "&&:", "\\0:", "\\1:", "\\2:", "\\3:",
  "\\4:", "\\5:", "\\6:", "\\7:", "\\8:", "\\9:",
  "\\A:", "\\B:", "\\C:", "\\D:", "\\E:", "\\F:"};

const char * const rrpls[] =
 {"\0", "&", "\\0", "\\1", "\\2", "\\3",
  "\\4", "\\5", "\\6", "\\7", "\\8", "\\9",
  "\\A", "\\B", "\\C", "\\D", "\\E", "\\F"};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TRegExpCalcFrm::ReDraw(TObject *Sender)
{
 int i, j, lTop, lHeight;
 char expr[BUFSIZE] = {"\0"};
 char src[BUFSIZE] = {"\0"};
 char replace[BUFSIZE] = {"\0"};
 strcpy(expr, CBExpr->Text.c_str());
 strcpy(src, CBSrc->Text.c_str());
 strcpy(replace, CBReplace->Text.c_str());
 if (RBRegExp->Checked)
  {
    TRegExp *RegExp = new TRegExp(expr);
    CBReplace->Enabled = true;
    CBReplace->Visible = true;
    LReplace->Visible = true;
    CBSrc->Width = Width - (CBSrc->Left*2);
    CBExpr->Width = Width - (CBExpr->Left*2);
    CBReplace->Width = Width - (CBReplace->Left*2);

    LRegExp->Caption = "RegExp:";
    LSrc->Caption = "Source:";
    CBCalc->Visible = false;
    bool match = false;
    AnsiString Str = "";
    if (RegExp)
     {
      if ((RegExp->RegError() == RE_OK)) match = RegExp->Match(src);
      else Str = asprintf("%s",RegExp->regerr()); //RegExp->RegErrorStr(Str);
     }
    lTop = CBReplace->Top + (CBReplace->Height*1.5);
    for(i = 0; i < _RLINES_; i++)
     {
      for(j = 0, lHeight = 0; j < _RCOLUMNS_; j++)
       if (LB[j][i])
       {
        LB[j][i]->Parent = this;
        LB[j][i]->Visible = true;
        if (j == 0) LB[j][i]->Caption = ttls[i];
        else
         {
          if (match)
           {
            char str[BUFSIZE] = {"\0"};
            if (rrpls[i][0] == '\0') RegExp->Replace(replace, str, BUFSIZE);
            else RegExp->Replace(rrpls[i], str, BUFSIZE);
            Str = asprintf("%s", str);
            LB[j][i]->Caption = Str;
           }
          else
           {
            if (rrpls[i][0] == '\0') LB[j][i]->Caption = Str;
            else LB[j][i]->Caption = "";
           }
         }
        LB[j][i]->Enabled = true;
        LB[j][i]->Tag = i+256*j;
        LB[j][i]->Top = lTop+2;

        if (j == 0)
         {
          LB[j][i]->Left = RBRegExp->Left;// 14;
          LB[j][i]->Font->Style = TFontStyles()<< fsBold;
          if (lHeight < LB[j][i]->Height) lHeight = LB[j][i]->Height;
         }
        else
         //LB[j][i]->Left = LB[0][i]->Width+40;
         LB[j][i]->Left = RBPrintf->Left;
       }
      lTop += lHeight;
     }
   Height = lTop + (LB[0][i]->Height*3)+20;
    delete RegExp;
  }
 else
 if (RBScanf->Checked)
  {
   char str[BUFSIZE] = {"\0"};
   CBReplace->Enabled = false;
   CBReplace->Visible = false;
   LReplace->Visible = false;
   LRegExp->Caption = "Data";
   LSrc->Caption = "Format";
   CBCalc->Visible = true;
   CBSrc->Width = Width - (CBSrc->Left*2);
   CBExpr->Width = Width - (CBExpr->Left*2);

    ssctst(str, src, expr);

    lTop = CBExpr->Top + (CBExpr->Height*1.5);
    for(i = 0; i < _RLINES_; i++)
     {
      for(j = 0, lHeight = 0; j < _RCOLUMNS_; j++)
       if (LB[j][i])
       {
        LB[j][i]->Parent = this;
        if (i == 0)
         {
          if (j == 0) LB[j][i]->Caption = ttls[i];
          else LB[j][i]->Caption = str;
         }
        else
         {
          LB[j][i]->Visible = false;
         }
        LB[j][i]->Enabled = true;
        LB[j][i]->Tag = i+256*j;
        LB[j][i]->Top = lTop+2;

        if (j == 0)
         {
          LB[j][i]->Left = 16;
          LB[j][i]->Font->Style = TFontStyles()<< fsBold;
          if (lHeight < LB[j][i]->Height) lHeight = LB[j][i]->Height;
         }
        else LB[j][i]->Left = LB[0][i]->Width+20;
       }
      if (i==0) lTop += lHeight;
     }
    Height = lTop + (LB[0][i]->Height*3)+20;

  }
 else
 if (RBPrintf->Checked)
  {
   char str[BUFSIZE] = {"\0"};
   CBReplace->Enabled = false;
   CBReplace->Visible = false;
   LReplace->Visible = false;
   LRegExp->Caption = "Parameters";
   LSrc->Caption = "Format";
   CBCalc->Visible = true;
   CBSrc->Width = Width - (CBSrc->Left*2);
   CBExpr->Width = Width - (CBExpr->Left*2);

   if (CBCalc->Checked)
     sprtst(str, src, expr, ccalc);
   else
     sprtst(str, src, expr, NULL);
    lTop = CBExpr->Top + (CBExpr->Height*1.5);
    for(i = 0; i < _RLINES_; i++)
     {
      for(j = 0, lHeight = 0; j < _RCOLUMNS_; j++)
       if (LB[j][i])
       {
        LB[j][i]->Parent = this;
        if (i == 0)
         {
          if (j == 0) LB[j][i]->Caption = ttls[i];
          else LB[j][i]->Caption = str;
         }
        else
         {
          LB[j][i]->Visible = false;
         }
        LB[j][i]->Enabled = true;
        LB[j][i]->Tag = i+256*j;
        LB[j][i]->Top = lTop+2;

        if (j == 0)
         {
          LB[j][i]->Left = 16;
          LB[j][i]->Font->Style = TFontStyles()<< fsBold;
          if (lHeight < LB[j][i]->Height) lHeight = LB[j][i]->Height;
         }
        else LB[j][i]->Left = LB[0][i]->Width+20;
       }
      if (i==0) lTop += lHeight;
     }
    Height = lTop + (LB[0][i]->Height*3)+20;
  }
}
//---------------------------------------------------------------------------
void __fastcall TRegExpCalcFrm::FormDestroy(TObject *Sender)
{
 delete ccalc;
 WriteIni();
 for(int i = 0; i < _RLINES_; i++)
  {
   for(int j = 0; j < _RCOLUMNS_; j++)
    {
     delete LB[j][i];
     LB[j][i] = NULL;
    }
  }
}

//---------------------------------------------------------------------------
void AddHistry(TStrings &List, AnsiString Str)
{
 int i;

 for(i = 0; i<List.Count; i++)
  if (List.Strings[i] == Str) break;
 if (i == List.Count) List.Insert(0, Str);
}
//---------------------------------------------------------------------------
void __fastcall TRegExpCalcFrm::FormShow(TObject *Sender)
{
 RBRegExp->Checked = true;
 RedIni();
 ReDraw(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TRegExpCalcFrm::CBExprChange(TObject *Sender)
{
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::CBExprEnter(TObject *Sender)
{
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::CBReplaceChange(TObject *Sender)
{
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::CBReplaceEnter(TObject *Sender)
{
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::CBReplaceExit(TObject *Sender)
{
 AddHistry(*CBReplace->Items, CBReplace->Text);
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::CBExprExit(TObject *Sender)
{
 AddHistry(*CBExpr->Items, CBExpr->Text);
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::CBSrcChange(TObject *Sender)
{
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::CBSrcEnter(TObject *Sender)
{
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::CBSrcExit(TObject *Sender)
{
 AddHistry(*CBSrc->Items, CBSrc->Text);
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::RedIni(void)
{
#ifdef _INI_
 TIniFile *ini;

 ini = new TIniFile(ChangeFileExt(Application->ExeName, ".INI"));

 RBRegExp->Checked = ini->ReadBool("TYPE", "RegExp", true);
 RBPrintf->Checked = ini->ReadBool("TYPE", "Printf", false);
 RBScanf->Checked = ini->ReadBool("TYPE", "Scanf", false);
 CBCalc->Checked = ini->ReadBool("TYPE", "Calc", true);
 CBSrc->Text = ini->ReadString("REC_SRC", "Text", "");
 for(int i = 0; i < 10; i++)
   CBSrc->Items->Add(ini->ReadString("REC_SRC", asprintf("Item_%i", i), ""));

 CBExpr->Text = ini->ReadString("REC_FIND", "Text", "");
 for(int i = 0; i < 10; i++)
   CBExpr->Items->Add(ini->ReadString("REC_FIND", asprintf("Item_%i", i), ""));

 CBReplace->Text = ini->ReadString("REC_RPLS", "Text", "");
 for(int i = 0; i < 10; i++)
   CBReplace->Items->Add(ini->ReadString("REC_RPLS", asprintf("Item_%i", i), ""));

 delete ini;
#else
 TRegistry *Reg = new TRegistry();

 try
 {
  Reg->RootKey =  HKEY_CURRENT_USER;
  Reg->Access = KEY_READ;
  if (Reg->OpenKey("\\SOFTWARE\\GTK\\RXCALC", false))
   {
    try
     {
      //Read values
      CBSrc->Text = Reg->ReadString("REG_SRC");
      CBExpr->Text = Reg->ReadString("REG_FIND");
      CBReplace->Text = Reg->ReadString("REG_RPLS");
      for(int i = 0; i < 10; i++)
       {
        CBSrc->Items->Add(Reg->ReadString(asprintf("REG_SRC_%i", i)));
        CBExpr->Items->Add(Reg->ReadString(asprintf("REG_FIND_%i", i)));
        CBReplace->Items->Add(Reg->ReadString(asprintf("REG_RPLS_%i", i)));
       }
     }
    catch(...)
     {
      //Set default
     }
   }
 }
 __finally
 {
  delete Reg;
 }
#endif
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::WriteIni(void)
{
#ifdef _INI_
 TIniFile *ini;

 ini = new TIniFile(ChangeFileExt(Application->ExeName, ".INI"));
 ini->WriteBool("TYPE", "RegExp", RBRegExp->Checked);
 ini->WriteBool("TYPE", "Scanf", RBScanf->Checked);
 ini->WriteBool("TYPE", "Printf", RBPrintf->Checked);
 ini->WriteBool("TYPE", "Calc", CBCalc->Checked);
 ini->WriteString("REC_SRC", "Text", CBSrc->Text);
 for(int i = 0; (i < CBSrc->Items->Count) && (i < 10); i++)
  if (CBSrc->Items->Strings[i] != "")
   ini->WriteString("REC_SRC", asprintf("Item_%i", i), CBSrc->Items->Strings[i]);

 ini->WriteString("REC_FIND", "Text", CBExpr->Text);
 for(int i = 0; (i < CBExpr->Items->Count) && (i < 10); i++)
  if (CBExpr->Items->Strings[i] != "")
   ini->WriteString("REC_FIND", asprintf("Item_%i", i), CBExpr->Items->Strings[i]);

 ini->WriteString("REC_RPLS", "Text", CBReplace->Text);
 for(int i = 0; (i < CBReplace->Items->Count) && (i < 10); i++)
  if (CBReplace->Items->Strings[i] != "")
   ini->WriteString("REC_RPLS", asprintf("Item_%i", i), CBReplace->Items->Strings[i]);

 delete ini;
#else
 TRegistry *Reg = new TRegistry();

 try
 {
  Reg->RootKey =  HKEY_CURRENT_USER;
  //Reg->Access = KEY_READ;
  Reg->Access = KEY_ALL_ACCESS;
  if (Reg->OpenKey("\\SOFTWARE\\GTK\\RXCALC", true))
   {
    try
     {
      //Write values
      Reg->WriteString("REG_SRC", CBSrc->Text);
      Reg->WriteString("REG_FIND", CBExpr->Text);
      Reg->WriteString("REG_RPLS", CBReplace->Text);
      for(int i = 0; i < 10; i++)
       {
        if ((i < CBSrc->Items->Count) && (CBSrc->Items->Strings[i] != ""))
         Reg->WriteString(asprintf("REG_SRC_%i", i), CBSrc->Items->Strings[i]);

        if ((i < CBExpr->Items->Count) && (CBExpr->Items->Strings[i] != ""))
         Reg->WriteString(asprintf("REG_FIND_%i", i), CBExpr->Items->Strings[i]);

        if ((i < CBReplace->Items->Count) && (CBReplace->Items->Strings[i] != ""))
         Reg->WriteString(asprintf("REG_RPLS_%i", i), CBReplace->Items->Strings[i]);
       }
     }
    catch(...)
     {
      //Set default
     }
   }
 }
 __finally
 {
  delete Reg;
 }
#endif
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 if (Key == VK_ESCAPE)
  {
   Key = 0;
   Close();
  }
 else
 if (Shift.Contains(ssAlt) &&(Key == 'X'))
  {
   Key = 0;
   Close();
  }
 else
 if (Key == VK_F1)
  {
   Key = 0;
   Application->HelpCommand(HELP_CONTENTS, 0);
  }

}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
 WriteIni();
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::RBPrintfClick(TObject *Sender)
{
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::RBRegExpClick(TObject *Sender)
{
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::FormCreate(TObject *Sender)
{
 ccalc = new calculator();
 ccalc->syntax(UPCASE+FFLOAT+SCI);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::CBCalcClick(TObject *Sender)
{
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::FormResize(TObject *Sender)
{
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRegExpCalcFrm::RBScanfClick(TObject *Sender)
{
 ReDraw(Sender);
}
//---------------------------------------------------------------------------

