"C:\Program Files (x86)\Borland\CBuilder6\Bin\make.exe" -ffcalc.mak -B
upx -9 fcalc.exe
"C:\Program Files (x86)\Borland\CBuilder6\Bin\make.exe" -fRegExpCalculator.mak -B
upx -9 RegExpCalculator.exe
pause
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" fcalc.iss
pause
call clean
REM "Setup\fcalc_setup.exe"
