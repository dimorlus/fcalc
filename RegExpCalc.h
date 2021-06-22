//---------------------------------------------------------------------------

#ifndef RegExpCalcH
#define RegExpCalcH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

//---------------------------------------------------------------------------
#define _RLINES_                 12+6
#define _RCOLUMNS_               2
//---------------------------------------------------------------------------
class TRegExpCalcFrm : public TForm
{
__published:	// IDE-managed Components
        TComboBox *CBSrc;
        TLabel *LSrc;
        TComboBox *CBExpr;
        TLabel *LRegExp;
        TComboBox *CBReplace;
        TLabel *LReplace;
        TRadioButton *RBRegExp;
        TRadioButton *RBPrintf;
        TCheckBox *CBCalc;
        TRadioButton *RBScanf;
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall CBExprChange(TObject *Sender);
        void __fastcall CBExprEnter(TObject *Sender);
        void __fastcall CBReplaceChange(TObject *Sender);
        void __fastcall CBReplaceEnter(TObject *Sender);
        void __fastcall CBReplaceExit(TObject *Sender);
        void __fastcall CBExprExit(TObject *Sender);
        void __fastcall CBSrcChange(TObject *Sender);
        void __fastcall CBSrcEnter(TObject *Sender);
        void __fastcall CBSrcExit(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall RBPrintfClick(TObject *Sender);
        void __fastcall RBRegExpClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall CBCalcClick(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall RBScanfClick(TObject *Sender);
private:	// User declarations
        calculator *ccalc;
public:		// User declarations
        TLabel *LB[_RCOLUMNS_][_RLINES_];
        void __fastcall ReDraw(TObject *Sender);
        void __fastcall RedIni(void);
        void __fastcall WriteIni(void);
        __fastcall TRegExpCalcFrm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRegExpCalcFrm *RegExpCalcFrm;
//---------------------------------------------------------------------------
#endif
