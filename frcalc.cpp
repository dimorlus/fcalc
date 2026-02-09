//---------------------------------------------------------------------------
#pragma warn -8058
#include <windows.h>
#include <vcl.h>
#include <string.h>
#include <IniFiles.hpp>
#include <Registry.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <math.hpp>
#include <system.hpp>
#include <HtmlHelp.h>
#pragma hdrstop

#include "ViewVars.h"
#include "frcalc.h"
#include "StrUtils.h"
#include "HTMLHelpViewer.hpp"
//---------------------------------------------------------------------------
#pragma warn -8080
#pragma warn -8071
#pragma warn -8004 // assigned a value that is never used

#pragma package(smart_init)
#pragma resource "*.dfm"
#pragma link "HTMLHelpViewer"
TCalcForm *CalcForm;

//---------------------------------------------------------------------------
int GetMainMenuHeight(void)
{
  NONCLIENTMETRICS Rec;

  Rec.cbSize = sizeof(Rec);
  if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, Rec.cbSize, &Rec.cbSize, 0))
	return Rec.iMenuHeight;
  else return 0;
}
//---------------------------------------------------------------------------
int GetCaptionHeight(void)
{
  NONCLIENTMETRICS Rec;

  Rec.cbSize = sizeof(Rec);
  if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, Rec.cbSize, &Rec.cbSize, 0))
	return Rec.iCaptionHeight;
  else return -1;
}

//---------------------------------------------------------------------------

__fastcall TCalcForm::TCalcForm(TComponent* Owner)
        : TForm(Owner)
{
 HelpFile = ExtractFilePath(Application->ExeName) + "fcalc.chm";
}
//---------------------------------------------------------------------------
/*
static void GetVars(char *Name, float_t fVal)
{
  CalcForm->Vars->Add(AnsiString().sprintf("%-10s=%g", Name, fVal));
}
*/
//---------------------------------------------------------------------------
//#define USE_calculator_format_out
void __fastcall TCalcForm::CBStrChange(TObject *Sender)
{
 __int64 iVal;
 int n = 0;
 int prnsize;
 char strings[2048];

 Caption = "Calc: " + CBStr->Text;

 Application->Title = CBStr->Text;

 Binarywidth->Caption = AnsiString().sprintf("Binary width = %u", binwide);
 float__t op = 100.0*(float__t)opacity/255.0;
 Opacity->Caption = AnsiString().sprintf("Opacity(%-1.0Lf)", op);

// SetExceptionMask(exAllArithmeticExceptions);
//  Set8087CW(0x133f);
//  SetExceptionMask(GetExceptionMask() << exZeroDivide << exInvalidOp << exDenormalized << exOverflow << exUnderflow << exPrecision);
 try
  {
   fVal = ccalc->evaluate(CBStr->Text.c_str(), &iVal, &imVal);
  }
 catch ( ... )
  {
  }

 int scfg = ccalc->issyntax();

 memset(strings, 0, sizeof(strings));
 n = ccalc->print(strings, Options,  binwide, &prnsize);
 if ((prnsize > 2) && (strings[prnsize - 2] == '\r')) strings[prnsize - 2] = '\0';
 if (n == 0) n++;

 {
   char str[128];
   int w, ww = 0;
   int ii, jj = 0;
   char c;
   Graphics::TBitmap* bm = new Graphics::TBitmap;
   bm->Canvas->Font = MOutput->Font;

   for(int i=0; (i < n) && (jj < prnsize - 2); i++)
    {
     ii = 0;
     do
      {
       c = str[ii++] = strings[jj++];
       if ((c == '\r')||(c == '\0')) break;
      }
     while (c);
     str[ii-1] = '\0';
     w = bm->Canvas->TextWidth(str);
     if (ww < w) ww = w;
     if (c == '\0') break;
    }
   delete bm;

   MOutput->Lines->Clear();
   int dpi = GetDeviceCaps(Canvas->Handle, LOGPIXELSX);
   int nw = ww+10*dpi/96;
   if (Width != nw) Width = nw;
 }

 SendMessage(MOutput->Handle, WM_VSCROLL, SB_TOP, 0);
 SendMessage(MOutput->Handle, WM_HSCROLL, SB_TOP, 0);

 MOutput->Text = strings;

 if (CalcForm->MnCalc->Enabled)
   Height = GetCaptionHeight()*1.2+
            CBStr->Height+10+
            GetMainMenuHeight()+
            ((CBStr->Height*0.7)*(n));
 else
   Height = GetCaptionHeight()*1.2+
            CBStr->Height+//10+
            ((CBStr->Height*0.7)*(n));
 SendMessage(MOutput->Handle, WM_VSCROLL, SB_TOP, 0);
 SendMessage(MOutput->Handle, WM_HSCROLL, SB_TOP, 0);
 SetOpt();
 Opt2Mnu();
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::FormDestroy(TObject *Sender)
{
 //delete Vars;
 delete ccalc;
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::AddHist(void)
{
 int i;
 AnsiString Str = CBStr->Text.Trim();

 if (Str == "") return;

 for(i = 0; i < CBStr->Items->Count; i++)
  if (CBStr->Items->Strings[i].AnsiCompareIC(Str) == 0) break;

 if (i == CBStr->Items->Count) CBStr->Items->Insert(0, Str);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::CBStrKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
//VK_LEFT	Left Arrow key
//VK_RIGHT	Right Arrow key
 if (Key == VK_UP) //	Up Arrow key
 {
  AddHist();
 }
 else
 if (Key == VK_DOWN) //	Down Arrow key
 {
  AddHist();
 }
 else
 if (Key == VK_NEXT) //	Page Down key
 {
  AddHist();
  Key = 0;
 }
 else
 if (Key == VK_PRIOR) //	Page Up key
 {
  AddHist();
  Key = 0;
 }
 else
 if (Key == VK_DELETE)
  {
   if (CBStr->DroppedDown)
    {
      Key = 0;
      CBStr->Items->Delete(CBStr->ItemIndex);
    }
  }
 else
 if (Shift.Contains(ssCtrl) && ((Key == 'r')||(Key=='R')))
  {
   CBStr->Text = "sqrt("+CBStr->Text+")";
   CBStrChange(Sender);
  }
 else
 if (Shift.Contains(ssCtrl) && ((Key == 'i')||(Key=='I')))
  {
   CBStr->Text = "1/("+CBStr->Text+")";
   CBStrChange(Sender);
  }
 else
 if (Shift.Contains(ssCtrl) && ((Key == 's')||(Key=='S')))
  {
   CBStr->Text = "("+CBStr->Text+")^2";
   CBStrChange(Sender);
  }
 else
 if (Shift.Contains(ssCtrl) && ((Key == 219)||(Key==221)))
  {
   CBStr->Text = "("+CBStr->Text+")";
   CBStrChange(Sender);
  }
 //else  AddHist();
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::CBStrKeyPress(TObject *Sender, char &Key)
{
 int i;
//VK_LEFT	Left Arrow key
//VK_RIGHT	Right Arrow key
/*
 if (Key == VK_UP) //	Up Arrow key
 {
  AddHist();
 }
 else
 if (Key == VK_DOWN) //	Down Arrow key
 {
  AddHist();
 }
 else
 if (Key == VK_NEXT) //	Page Down key
 {
  AddHist();
  Key = 0;
 }
 else
 if (Key == VK_PRIOR) //	Page Up key
 {
  AddHist();
  Key = 0;
 }
 else
*/
 if (Key == VK_RETURN)
  {
   Key = 0;
   AddHist();
   if (CBStr->DroppedDown)
    {
     CBStr->DroppedDown = false;
     int i = CBStr->ItemIndex;
     if (CBStr->Items->Objects[i] == NULL)
       CBStr->Text = CBStr->Items->Strings[i];
    }
 else
 if (Key == VK_ESCAPE)
  {
   if (CBStr->DroppedDown)
    {
     Key = 0;
     CBStr->DroppedDown = false;
    }
  }
 else
   {
    AddHist();
    CBStr->Text = asprintf("%.16Lg", fVal);
    CBStr->SelLength = CBStr->Text.Length();
   }
  }
}
//---------------------------------------------------------------------------

float__t Help(float__t d)
{
 Application->HelpCommand(HELP_CONTENTS, 0);
 return 0;
}
//---------------------------------------------------------------------------

float__t menu(float__t d)
{
 if (d == 0)
  {
   CalcForm->MnCalc->Visible = false;
   CalcForm->MnCalc->Enabled = false;
   CalcForm->Options &= ~MNU;
  }
 else
  {
   CalcForm->MnCalc->Visible = true;
   CalcForm->MnCalc->Enabled = true;
   CalcForm->Options |= MNU;
  }

 return 0;
}
//---------------------------------------------------------------------------

float__t home(float__t x, float__t y)
{
 if (x < 0) x = 0;
 if (y < 0) y = 0;
 if (x > 8192) x = 0;
 if (y > 8192) y = 0;
 CalcForm->Top = (int)x;
 CalcForm->Left = (int)y;
 return 0;
}
//---------------------------------------------------------------------------

float__t vars(float__t d)
{
 CalcForm->Variables1Click(NULL);
 return 0;
}
//---------------------------------------------------------------------------

float__t fOpacity(float__t d)
{
 d = d * 255.0/100;
 if (d < 20.0) d = 20.0;
 CalcForm->opacity = (int)d;
 CalcForm->AlphaBlend = true;
 CalcForm->AlphaBlendValue = (int)d;
 return 0;
}
//---------------------------------------------------------------------------

float__t BinWide(float__t d)
{
 if (d > 64.0) d = 64.0;
 CalcForm->binwide = (int)d;
 return 0;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TCalcForm::FormCreate(TObject *Sender)
{
 ccalc = new calculator();
#ifdef INI_FILE
 TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
 Top = Reg->ReadInteger("Top");
 Left = ini->ReadInteger("Left");
 opacity = ini->ReadInteger("Opacity");
 binwide = ini->ReadInteger("BinWide");
 Options = ini->ReadInteger("Options");
 for(int i = 0; (i < 30); i++)
  {
   Hist = Reg->ReadString(AnsiString().sprintf("HIST%02d", i));
   if (Hist != "") CBStr->Items->AddObject(Hist.Trim(), NULL);
   else break;
  }
 CBStr->Text = Reg->ReadString("Str").Trim();
 CBStr->SelLength = CBStr->Text.Length();
 delete ini;
 Opt2Mnu();
#else
 TRegistry *Reg = new TRegistry();
 //Vars = new TStringList();
 AnsiString Hist;
  try
  {
   //Reg->RootKey =  HKEY_LOCAL_MACHINE;
   Reg->RootKey =  HKEY_CURRENT_USER;
   Reg->Access = KEY_READ;
   //Reg->Access = KEY_ALL_ACCESS;
   if (Reg->OpenKey("\\SOFTWARE\\MLT\\SCALC", false))
    {
     try
      {
       Top = Reg->ReadInteger("Top");
       Left = Reg->ReadInteger("Left");
       opacity = Reg->ReadInteger("Opacity");
       binwide = Reg->ReadInteger("BinWide");
       Options = Reg->ReadInteger("Options");
      }
     catch(...)
      {
       Top = 200;
       Left = 200;
       Casesensitive->Checked = false;
       Options = FFLOAT+SCI+NRM+CMP+IGR+UNS+HEX+CHR+WCH+OCT+fBIN+DAT+DEG+STR+ALL+MNU+FRC+FRI;
       opacity = 255;
       binwide = 64;
      }
     for(int i = 0; (i < 30); i++)
      {
       Hist = Reg->ReadString(AnsiString().sprintf("HIST%02d", i));
       if (Hist != "") CBStr->Items->AddObject(Hist.Trim(), NULL);
       else break;
      }
     CBStr->Text = Reg->ReadString("Str").Trim();
     CBStr->SelLength = CBStr->Text.Length();
     Reg->CloseKey();
     Opt2Mnu();
     //CBStr->AutoComplete = false;
    }
  }
  __finally
  {
   delete Reg;
  }
#endif
 ccalc->addfn("help", (void*)(float__t(*)(float__t))Help);
 ccalc->addfn("menu", (void*)(float__t(*)(float__t))menu);
 ccalc->addfn("vars", (void*)(float__t(*)(float__t))vars);
 ccalc->addfn("opacity", (void*)(float__t(*)(float__t))fOpacity);
 ccalc->addfn("binwide", (void*)(float__t(*)(float__t))BinWide);
 ccalc->addfn2("home", (void*)(float__t(*)(float__t, float__t))home);
 //AlphaBlend = true;
 //AlphaBlendValue = opacity;
 Application->ShowHint = true;
 SetOpt(true);
 //SaveHist = CBStr->Items;

}

//---------------------------------------------------------------------------


void __fastcall TCalcForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 AddHist();
 TRegistry *Reg = new TRegistry();
  try
  {
   //Reg->RootKey =  HKEY_LOCAL_MACHINE;
   Reg->RootKey =  HKEY_CURRENT_USER;
   Reg->Access = KEY_ALL_ACCESS;
   if (Reg->OpenKey("\\SOFTWARE\\MLT\\SCALC", true))
    {
     //CBStr->Items = SaveHist;
     for(int i = 0; i < 30; i++)
      if ((i < CBStr->Items->Count) &&
          (CBStr->Items->Objects[i] == NULL)) //History record
       Reg->WriteString(AnsiString().sprintf("HIST%02d", i),
         CBStr->Items->Strings[i]);
      else
       Reg->WriteString(AnsiString().sprintf("HIST%02d", i), "");

     Reg->WriteString("Str", CBStr->Text);
     Reg->WriteInteger("Top", Top);
     Reg->WriteInteger("Left", Left);
     Reg->WriteInteger("Opacity", opacity);
     Reg->WriteInteger("BinWide", binwide);
     Reg->WriteInteger("Options", Options);
     Reg->CloseKey();
    }
  }
  __finally
  {
   delete Reg;
  }

}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::CstyleClick(TObject *Sender)
{
 int opt = ccalc->issyntax();
 opt ^= PAS;
 ccalc->syntax(opt);
 SetOpt();
 CBStrChange(Sender);
 Options |= ccalc->issyntax() & PAS;
}

//---------------------------------------------------------------------------

void __fastcall TCalcForm::CasesensitiveClick(TObject *Sender)
{
 Casesensitive->Checked ^= 1;
 SetOpt();
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::SetOpt(bool forced)
{
 int opt = ccalc->issyntax()|(Options & (SCI+FRI));

 if (forced)
  {
   if (Options & PAS) opt |= PAS;
   else opt &= ~PAS;
  }
 else
  {
   if (opt & PAS) Cstyle->Caption = "&Pas-style";
   else Cstyle->Caption = "&C-style";
  }

 Options |= opt & PAS;

 if (Casesensitive->Checked) opt &= ~UPCASE;
 else opt |= UPCASE;

 if (Forcedfloat->Checked) opt |= FFLOAT;
 else opt &= ~FFLOAT;

 Cstyle->Checked = true;

 ccalc->syntax(opt & (PAS+UPCASE+FFLOAT+FRI)|(SCI));
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::Exit1Click(TObject *Sender)
{
 Close();
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::Variables1Click(TObject *Sender)
{
 char vars[4096];
 int MaxLen = 0;
 VarsFrm->Top = Top;
 VarsFrm->Left = Left;
 VarsFrm->VarsMemo->Lines->Clear();
 ccalc->varlist(vars, sizeof(vars), &MaxLen);
 VarsFrm->VarsMemo->Lines->Text = vars;
 VarsFrm->ShowModal();
}

//---------------------------------------------------------------------------

void __fastcall TCalcForm::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  if (Key == VK_F1)
  {
   Key = 0;
   Application->HelpCommand(HELP_CONTENTS, 3005);
  }
 else
  if (Key == VK_ESCAPE)
  {
   if (!CBStr->DroppedDown)
    {
     Key = 0;
     if (Escmin->Checked) Application->Minimize();
     else Close();
    }
  }
 else
 if (Shift.Contains(ssAlt) &&(Key == 'X'))
  {
   Key = 0;
   Close();
  }
 else
 if ((Shift.Contains(ssAlt) || Shift.Contains(ssCtrl)) && (Key ==  VK_HOME))
  {
   Key = 0;
   Top = 200;
   Left = 200;
   opacity = 255;
   CalcForm->AlphaBlendValue = opacity;
  }
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::CBStrSelect(TObject *Sender)
{
 CBStr->SelLength = CBStr->Text.Length();
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::FormShow(TObject *Sender)
{
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::EscminClick(TObject *Sender)
{
 Escmin->Checked ^= 1;
 if (Escmin->Checked) Options |= MIN;
 else Options &= ~MIN;
}
//---------------------------------------------------------------------------


void __fastcall TCalcForm::ForcedfloatClick(TObject *Sender)
{
 Forcedfloat->Checked ^= 1;
 if (Forcedfloat->Checked) Options |= FFLOAT;
 else Options &= ~FFLOAT;

 SetOpt();
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::ScientificClick(TObject *Sender)
{
 Scientific->Checked ^= 1;
 if (Scientific->Checked) Options |= SCI;
 else Options &= ~SCI;

 All->Checked = false;
 Options &= ~ALL;

 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::NormalizedClick(TObject *Sender)
{
 Normalized->Checked ^= 1;
 if (Normalized->Checked) Options |= NRM;
 else Options &= ~NRM;

 All->Checked = false;
 Options &= ~ALL;

 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::ComputingClick(TObject *Sender)
{
 Computing->Checked ^= 1;
 if (Computing->Checked) Options |= CMP;
 else Options &= ~CMP;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::IntegerClick(TObject *Sender)
{
 Integer->Checked ^= 1;
 if (Integer->Checked) Options |= IGR;
 else Options &= ~IGR;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::UnsignedClick(TObject *Sender)
{
 Unsigned->Checked ^= 1;
 if (Unsigned->Checked) Options |= UNS;
 else Options &= ~UNS;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::HexClick(TObject *Sender)
{
 Hex->Checked ^= 1;
 if (Hex->Checked) Options |= HEX;
 else Options &= ~HEX;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::OctalClick(TObject *Sender)
{
 Octal->Checked ^= 1;
 if (Octal->Checked) Options |= OCT;
 else Options &= ~OCT;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::BinaryClick(TObject *Sender)
{
 Binary->Checked ^= 1;
 if (Binary->Checked) Options |= fBIN;
 else Options &= ~fBIN;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::DatetimeClick(TObject *Sender)
{
 Datetime->Checked ^= 1;
 if (Datetime->Checked) Options |= DAT;
 else Options &= ~DAT;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::UnixtimeClick(TObject *Sender)
{
 Unixtime->Checked ^= 1;
 if (Unixtime->Checked) Options |= UTM;
 else Options &= ~UTM;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::DegreesClick(TObject *Sender)
{
 Degrees->Checked ^= 1;
 if (Degrees->Checked) Options |= DEG;
 else Options &= ~DEG;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::CharClick(TObject *Sender)
{
 Char->Checked ^= 1;
 if (Char->Checked) Options |= CHR;
 else Options &= ~CHR;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TCalcForm::WCharClick(TObject *Sender)
{
 WChar->Checked ^= 1;
 if (WChar->Checked) Options |= WCH;
 else Options &= ~WCH;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}

//---------------------------------------------------------------------------
void __fastcall TCalcForm::StringClick(TObject *Sender)
{
 String->Checked ^= 1;
 if (String->Checked) Options |= STR;
 else Options &= ~STR;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::FractionClick(TObject *Sender)
{
 Fraction->Checked ^= 1;
 if (Fraction->Checked) Options |= FRC;
 else Options &= ~FRC;

 All->Checked = false;
 Options &= ~ALL;

 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::InchClick(TObject *Sender)
{
 int opt = ccalc->issyntax();
 Inch->Checked ^= 1;
 if (Inch->Checked)
  {
   Options |= FRI;
   opt |= FRI;
  }
 else
  {
   Options &= ~FRI;
   opt &= ~FRI;
  }
 All->Checked = false;
 Options &= ~ALL;
 ccalc->syntax(opt);
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TCalcForm::AllClick(TObject *Sender)
{
 All->Checked ^= 1;
 if (All->Checked) Options |= ALL;
 else Options &= ~ALL;

 Scientific->Checked = All->Checked;
 Normalized->Checked = All->Checked;
 Computing->Checked = All->Checked;
 Integer->Checked = All->Checked;
 Unsigned->Checked = All->Checked;
 Hex->Checked = All->Checked;
 Char->Checked = All->Checked;
 WChar->Checked = All->Checked;
 Octal->Checked = All->Checked;
 Binary->Checked = All->Checked;
 Datetime->Checked = All->Checked;
 Unixtime->Checked = All->Checked;
 Degrees->Checked = All->Checked;
 String->Checked = All->Checked;
 Fraction->Checked = All->Checked;
 Inch->Checked = All->Checked;

 if (Escmin->Checked) Options |= MIN;
 else Options &= ~MIN;
 if (Forcedfloat->Checked) Options |= FFLOAT;
 else Options &= ~FFLOAT;
 if (Scientific->Checked) Options |= SCI;
 else Options &= ~SCI;
 if (Normalized->Checked) Options |= NRM;
 else Options &= ~NRM;
 if (Computing->Checked) Options |= CMP;
 else Options &= ~CMP;
 if (Integer->Checked) Options |= IGR;
 else Options &= ~IGR;
 if (Unsigned->Checked) Options |= UNS;
 else Options &= ~UNS;
 if (Hex->Checked) Options |= HEX;
 else Options &= ~HEX;
 if (Octal->Checked) Options |= OCT;
 else Options &= ~OCT;
 if (Binary->Checked) Options |= fBIN;
 else Options &= ~fBIN;
 if (Datetime->Checked) Options |= DAT;
 else Options &= ~DAT;
 if (Unixtime->Checked) Options |= UTM;
 else Options &= ~UTM;
 if (Degrees->Checked) Options |= DEG;
 else Options &= ~DEG;
 if (Char->Checked) Options |= CHR;
 else Options &= ~CHR;
 if (WChar->Checked) Options |= WCH;
 else Options &= ~WCH;
 if (String->Checked) Options |= STR;
 else Options &= ~STR;
 if (Fraction->Checked) Options |= FRC;
 else Options &= ~FRC;
 if (Inch->Checked) Options |= FRI;
 else Options &= ~FRI;

 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::N8Click(TObject *Sender)
{
 binwide = 8;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::N16Click(TObject *Sender)
{
 binwide = 16;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::N24Click(TObject *Sender)
{
 binwide = 24;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::N32Click(TObject *Sender)
{
 binwide = 32;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::N48Click(TObject *Sender)
{
 binwide = 48;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::N64Click(TObject *Sender)
{
 binwide = 64;
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::binwide64Click(TObject *Sender)
{
 CBStr->Text="binwide(64)";
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TCalcForm::OpacityClick(TObject *Sender)
{
 CBStr->Text="opacity(100)";
 CBStrChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TCalcForm::Opt2Mnu(void)
{
 Escmin->Checked = Options & MIN;
 Forcedfloat->Checked = Options & FFLOAT;
 Scientific->Checked = Options & SCI;
 Normalized->Checked = Options & NRM;
 Computing->Checked = Options & CMP;
 Integer->Checked = Options & IGR;
 Unsigned->Checked = Options & UNS;
 Hex->Checked = Options & HEX;
 Octal->Checked = Options & OCT;
 Binary->Checked = Options & fBIN;
 Datetime->Checked = Options & DAT;
 Degrees->Checked = Options & DEG;
 Char->Checked = Options & CHR;
 WChar->Checked = Options & WCH;
 String->Checked = Options & STR;
 Fraction->Checked = Options & FRC;
 Inch->Checked = Options & FRI;
 Temperature->Checked = Options & FRH;
 if (Options & MNU)
  {
   CalcForm->MnCalc->Visible = true;
   CalcForm->MnCalc->Enabled = true;
  }
 else
  {
   CalcForm->MnCalc->Visible = false;
   CalcForm->MnCalc->Enabled = false;
  }
}
//---------------------------------------------------------------------------


void __fastcall TCalcForm::CBStrKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 //AddHist();
}
//---------------------------------------------------------------------------




void __fastcall TCalcForm::TemperatureClick(TObject *Sender)
{
 Temperature->Checked ^= 1;
 if (Temperature->Checked) Options |= FRH;
 else Options &= ~FRH;

 All->Checked = false;
 Options &= ~ALL;
 CBStrChange(Sender);
}

