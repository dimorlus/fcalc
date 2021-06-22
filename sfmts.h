//---------------------------------------------------------------------------

#ifndef sfmtsH
#define sfmtsH
//---------------------------------------------------------------------------

extern int t2str(char * str, __int64 sec);
extern int b2str(char *str, const char *fmt, unsigned __int64 b);
extern int d2scistr(char *str, float_t d);
extern int d2frcstr(char *str, float_t d, int eps_order);
extern int b2scistr(char *str, float_t d);
extern int d2nrmstr(char *str, float_t d);
extern int dgr2str(char *str, float_t d);
extern int chr2str(char *str, unsigned char c);
extern int wchr2str(char *str, int i);
extern int nx_time2str(char *str, __int64 time);
extern void fraction(double val, double eps, int &num, int &denum);

#endif
