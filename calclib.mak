# ---------------------------------------------------------------------------
!if !$d(BCB)
BCB = $(MAKEDIR)\..
!endif

# ---------------------------------------------------------------------------
# PROJECT: calclib_bcb.dll
# ---------------------------------------------------------------------------
VERSION = BCB.06.00
PROJECT = calclib_bcb.dll
OBJFILES = calclib.obj sfmts.obj scalc.obj sfunc.obj
RESFILES = 
MAINSOURCE = calclib.cpp
RESDEPEN = $(RESFILES)
LIBFILES = 
IDLFILES = 
IDLGENFILES = 
# Оставляем только системную библиотеку рантайма
LIBRARIES = rtl.lib
PACKAGES = rtl.bpi
SPARELIBS = rtl.lib
# Указываем ваш DEF-файл для правильного экспорта имен
DEFFILE = calclib.def
OTHERFILES = 
# ---------------------------------------------------------------------------
# Флаги компилятора
# -tWD (DLL), -u- (без подчеркиваний), -O2 (оптимизация)
# ---------------------------------------------------------------------------
#CFLAG1 = -O2 -Hc -Vx -Ve -X- -a8 -6 -b- -k- -vi -tWD -tWM -u- -c \
#    -D__BORLANDC__ -DCALCLIB_EXPORTS -D_RTLDLL -D_NO_VCL
#CFLAG1 = -O2 -Hc -Vx -Ve -X- -a8 -6 -b- -k- -vi -tWD -tWM -u- -c \
#    -D__BORLANDC__ -DCALCLIB_EXPORTS -D_RTLDLL -D_NO_VCL \
#    -D_STLP_NO_CUSTOM_IO -D_STLP_NO_OWN_IOSTREAMS -D_STLP_HAS_NO_NAMESPACES
	
CFLAG1 = -O2 -Hc -Vx -Ve -X- -a8 -6 -b- -k- -vi -tWD -tWM -u- -c \
    -D__BORLANDC__ -DCALCLIB_EXPORTS -D_RTLDLL -D_NO_VCL \
    -D_STLP_OWN_IOSTREAMS=0 -D_STLP_NO_REDEFINE_STD	
IDLFLAGS = -I$(BCB)\include -I$(BCB)\include\vcl -src_suffix cpp -D__BORLANDC__ -boa
PFLAGS = -$Y- -$L- -$D- -$A8 -v -JPHNE -M
RFLAGS = 
# -Tpd (Target Windows DLL), -Gi (Generate import lib)
LFLAGS = -L$(BCB)\lib\obj;$(BCB)\lib -D"" -aa -Tpd -x -Gn -Gi
# ---------------------------------------------------------------------------
# Списки путей
# ---------------------------------------------------------------------------
LIBPATH = $(BCB)\lib\obj;$(BCB)\lib
#INCLUDEPATH = $(BCB)\include;$(BCB)\include\vcl
INCLUDEPATH = $(BCB)\include\vcl;$(BCB)\include
# ---------------------------------------------------------------------------
# Программы сборки
# ---------------------------------------------------------------------------
BCC32 = $(BCB)\BIN\bcc32
LINKER = ilink32
# ---------------------------------------------------------------------------
# Стартовый объект для DLL
# ---------------------------------------------------------------------------
ALLOBJ = c0d32.obj $(OBJFILES)
ALLLIB = $(LIBFILES) $(LIBRARIES) import32.lib cw32mti.lib
# ---------------------------------------------------------------------------
# ПРАВИЛА СБОРКИ
# ---------------------------------------------------------------------------
$(PROJECT): $(OBJFILES) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) -L$(LIBPATH) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES)
!

.cpp.obj:
    $(BCC32) $(CFLAG1) -I$(INCLUDEPATH) -n$(@D) {$< }

.c.obj:
    $(BCC32) $(CFLAG1) -I$(INCLUDEPATH) -n$(@D) {$< }
# ---------------------------------------------------------------------------