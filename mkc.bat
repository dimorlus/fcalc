"C:\Program Files (x86)\Borland\CBuilder6\Bin\make.exe" -ffcalc.mak -B
upx -9 fcalc.exe
"C:\Program Files (x86)\Borland\CBuilder6\Bin\make.exe" -fRegExpCalculator.mak -B
upx -9 RegExpCalculator.exe
"C:\Program Files (x86)\Inno Setup 5\ISCC.exe" fcalc.iss
call clean
"Setup\fcalc_setup.exe"
