//---------------------------------------------------------------------------
//-< CCALC.H >-------------------------------------------------------*--------*
// Ccalc                      Version 1.01       (c) 1998  GARRET    *     ?  *
// (C expression command line calculator)                            *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Oct-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 20-Oct-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
#ifndef scalcH
#define scalcH

// RW - set both by calc engine and application
// WO - set only from application
// RO - set only by calc engine
// UI - set and used only from application
#define PAS     (1<<0)  // (RW) Pascal assingment and comparison style (:= =) <-> (= ==)
#define SCI     (1<<1)  // (WO) Scientific numbers format (2k == 2000)
#define UPCASE  (1<<2)  // (WO) Case insensetive variables
#define UTMP    (1<<3)  // (WO) Using $n for temp
#define FFLOAT  (1<<4)  // (WO) Forced float

#define DEG     (1<<5)  // (RO) Degrees format found
#define SCF     (1<<6)  // (RO) Scientific (6.8k) format found
#define ENG     (1<<7)  // (RO) Engineering (6k8) format found
#define STR     (1<<8)  // (RO) String format found
#define HEX     (1<<9)  // (RO) Hex format found
#define OCT     (1<<10) // (RO) Octal format found
#define fBIN    (1<<11) // (RO) Binary format found
#define DAT     (1<<12) // (RO) Date time format found
#define CHR     (1<<13) // (RO) Char format found
#define WCH     (1<<14) // (RO) WChar format found
#define ESC     (1<<15) // (RO) Escape format found
#define CMP     (1<<16) // (RO) Computing format found

#define NRM     (1<<17) // (UI) Normalized output
#define IGR     (1<<18) // (UI) Integer output
#define UNS     (1<<19) // (UI) Unsigned output
#define ALL     (1<<20) // (UI) All outputs
#define MIN     (1<<21) // (UI) Esc minimized feature
#define MNU     (1<<22) // (UI) Show/hide menu feature
#define UTM     (1<<23) // (RO) Unix time
#define FRC     (1<<24) // (UI) Fraction output
#define FRI     (1<<25) // (UI) Fraction inch output
#define AUTO    (1<<26) // (UI) Auto output
#define STRBUF  256     // bufer size for string operations

#define _WCHAR_         // L'c' and 'c'W input format allow

#define _long_double_

typedef __int64 int_t;
typedef unsigned __int64 unsigned_t;
#ifdef _long_double_
typedef long double float_t;
#else
typedef double float_t;
#endif
class value;
class symbol;

enum t_value
{
  tvINT,
  tvFLOAT,
  tvPERCENT,
  tvSTR
};

enum t_operator
{
  toBEGIN, toOPERAND, toERROR, toEND,
  toLPAR, toRPAR, toFUNC, toPOSTINC, toPOSTDEC, toFACT,
  toPREINC, toPREDEC, toPLUS, toMINUS, toNOT, toCOM,
  toPOW,
  toMUL, toDIV, toMOD, toPAR,
  toADD, toSUB,
  toASL, toASR, toLSR,
  toGT, toGE, toLT, toLE,
  toEQ, toNE,
  toAND,
  toXOR,
  toOR,
  toSET, toSETADD, toSETSUB, toSETMUL, toSETDIV,
  toSETMOD, toSETASL, toSETASR, toSETLSR,
  toSETAND, toSETXOR, toSETOR, toSETPOW,
  toSEMI,
  toCOMMA,
  toTERMINALS
};

#define BINARY(opd) (opd >= toPOW)

enum t_symbol
{
  tsVARIABLE,
  tsIFUNC1, //int f(int x)
  tsIFUNC2, //int f(int x, int y)
  tsFFUNC1, //float f(float x)
  tsFFUNC2, //float f(float x, float y)
  tsPFUNCn, //int printf(char *format, ...)
  tsSIFUNC1 //int f(char *s)
};

class value
{
  public:
    t_value tag;
    symbol* var;
    int     pos;
    #ifdef _long_double_
    struct
    #else /*_long_double_*/
    union
    #endif /*_long_double_*/
     {
      int_t  ival;
      float_t fval;
     };
    char *sval;

    inline value()
     {
      tag = tvINT;
      ival = 0;
      sval = NULL;
     }

    inline ~value()
     {
      if ((tag == tvSTR) && sval) free(sval);
      sval = NULL;
     }
    inline float_t get()
    {
      return tag == tvINT ? (float_t)ival : fval;
    }

    inline int_t get_int()
    {
      return tag == tvINT ? ival : (int_t)fval;
    }

    inline char*get_str()
    {
      return tag == tvSTR ? sval:NULL;
    }
};

class symbol
{
  public:
    t_symbol tag;
    void*    func;
    value    val;
    char*    name;
    symbol*  next;
};


const int max_stack_size = 256;
const int max_expression_length = 1024;

const int hash_table_size = 1013;


class calculator
{
  private:
    int  scfg;
    value v_stack[max_stack_size];
    symbol* hash_table[hash_table_size];
    int   v_sp;
    int   o_stack[max_stack_size];
    int   o_sp;
    char* buf;
    int   pos;
    int   tmp_var_count;
    char  err[256];
    char  sres[STRBUF];
    int   errpos;

    inline unsigned string_hash_function(char* p);
    symbol* add(t_symbol tag, const char* name, void* func = NULL);
    t_operator scan(bool operand, bool percent);
    void  error(int pos, const char* msg);
    inline void  error(const char* msg) {error(pos-1, msg);}
    bool  assign();

    int hscanf(char* str, int_t &ival, int &nn);
    int bscanf(char* str, int_t &ival, int &nn);
    int oscanf(char* str, int_t &ival, int &nn);
    int xscanf(char* str, int len, int_t &ival, int &nn);
    float_t dstrtod(char *s, char **endptr);
    float_t tstrtod(char *s, char **endptr);
    void engineering(float_t mul, char * &fpos, float_t &fval);
    void scientific(char * &fpos, float_t &fval);

  public:
    calculator(int cfg = PAS + SCI + UPCASE);
    inline void syntax(int cfg = PAS + SCI + UPCASE + FFLOAT) {scfg = cfg;}
    inline int issyntax(void) {return scfg;}
    inline char *error(void) {return err;}
    inline int errps(void) {return errpos;};
    void addfvar(const char* name, float_t val);
    void addivar(const char* name, int_t val);
    bool checkvar(const char* name);
    void addfn(const char* name, void *func) {add(tsFFUNC1, name, func);}
    void varlist(void (*f)(char*, float_t));
    void varlist(void (*f)(char*, value*));
    float_t evaluate(char* expr, __int64 *piVal = NULL);
    char *Sres(void) {return sres;};
    ~calculator(void);
};

extern bool IsNaN(const double fVal);
extern bool IsNaNL(const long double ldVal);
#define isnan(a) (a != a)

#endif



