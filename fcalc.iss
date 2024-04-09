; -- TesterAll.iss --

[Setup]
AppName=fcalc
AppVerName=fcalc (Scientific formula calculator) 1.0 files
VersionInfoVersion=1.0.0.0
AppContact=http://dorlov.no-ip.com
DefaultDirName={commonpf}\fcalc
DefaultGroupName=fcalc
UninstallDisplayIcon={app}\fcalc.exe
LicenseFile=fclic.txt
Compression=lzma/ultra
SolidCompression=yes
OutputDir=Setup
OutputBaseFilename=fcalc_setup
CreateUninstallRegKey=yes
PrivilegesRequired=admin

[Types]
Name: "custom"; Description: "custom"; Flags:iscustom

[Components]
; Note: The following line does nothing other than provide a visual cue
; to the user that the program files are installed no matter what.
Name: "program"; Description: "Fcalc formula calculator"; Types: custom; Flags: fixed
Name: "regexp"; Description:"RegExpCalculator regular expression evaluator"; Types: custom


[Files]
Source: "fcalc.exe"; DestDir: "{app}";Components: program; Flags:ignoreversion
Source: "fcalc.chm"; DestDir: "{app}";Components: program; Flags:ignoreversion
Source: "RegExpCalculator.exe"; DestDir: "{app}";Components: regexp; Flags:ignoreversion
Source: "RegExpCalc.chm"; DestDir: "{app}";Components: regexp; Flags:ignoreversion

[Registry]
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: string; Valuename:"Str"; ValueData:"L:=130u;c:=2.2n;f:=1/(2*pi*sqrt(l*c))"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: string; Valuename:"HIST00"; ValueData:"L:=130u"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: string; Valuename:"HIST01"; ValueData:"c:=2.2n"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: string; Valuename:"HIST02"; ValueData:"f:=1/(2*pi*sqrt(l*c))"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: string; Valuename:"HIST03"; ValueData:"opacity(100)"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: string; Valuename:"HIST04"; ValueData:"help(0)"; Components:program; Flags: createvalueifdoesntexist
;
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Top"; ValueData:"200"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Left"; ValueData:"200"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"PAS"; ValueData:"1"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"UPCASE"; ValueData:"0"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"ESCMIN"; ValueData:"1"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"FFLOAT"; ValueData:"1"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Scientific"; ValueData:"1"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Computing"; ValueData:"0"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Integer"; ValueData:"1"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Unsigned"; ValueData:"0"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Hex"; ValueData:"1"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Char"; ValueData:"0"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"WChar"; ValueData:"0"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Octal"; ValueData:"0"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Binary"; ValueData:"0"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Datetime"; ValueData:"0"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Degrees"; ValueData:"0"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"All"; ValueData:"0"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"Opacity"; ValueData:"200"; Components:program; Flags: createvalueifdoesntexist
Root:HKCU; Subkey: "Software\MLT\SCALC"; Valuetype: dword; Valuename:"BinWide"; ValueData:"64"; Components:program; Flags: createvalueifdoesntexist


[UninstallDelete]
Type: filesandordirs; Name: "{app}\src"
Type: dirifempty; Name: "{app}"


[Icons]
Name: "{group}\fcalc"; Filename: "{app}\fcalc.exe"; WorkingDir: "{app}";Components: program
Name: "{group}\RegExpCalculator"; Filename: "{app}\RegExpCalculator.exe"; WorkingDir: "{app}";Components: regexp
Name: "{group}\Uninstall"; Filename: "{app}\unins000.exe"
Name: "{userdesktop}\fcalc"; Filename: "{app}\fcalc.exe"; WorkingDir: "{app}";Components: program
Name: "{userdesktop}\RegExpCalc"; Filename: "{app}\RegExpCalculator.exe"; WorkingDir: "{app}";Components: regexp

[Run]
Filename: "{app}\fcalc.exe"; Description: "Launch application"; Flags: postinstall nowait skipifsilent unchecked
