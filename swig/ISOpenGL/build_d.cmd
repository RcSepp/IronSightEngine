del *.cs *.c *.cxx *.obj IronSightEngine.dll IronSightEngine_wrap.dll *.exp *.ilk *.lib *.pdb *.idb
swig -csharp -c++ -D_WIN32 -dllimport IronSightEngine_wrap ISOpenGL.i
REM copy *.cs K:\C#\SwigTest\SwigTest\
cl -c ISOpenGL_wrap.cxx -I../../include/ /ZI /nologo /W3 /WX- /Od /Oy- /D "_WINDLL" /D "_MBCS" /Gm /EHsc /RTC1 /GS /fp:precise /Zc:wchar_t /Zc:forScope /Fp"IronSightEngine_wrap.pch" /Fd"vc100.pdb" /Gd /analyze- /errorReport:queue /MDd
link ISOpenGL_wrap.obj /OUT:"IronSightEngine_wrap.dll" /NOLOGO /DLL "K:\C++\IronSightEngine\lib\x86\ISEngine_d.lib" "K:\C++\IronSightEngine\lib\x86\ISMath_d.lib" "K:\C++\IronSightEngine\dll\x86\ISOpenGL_d.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /DEBUG /PDB:"IronSightEngine_wrap.pdb" /TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X86 /ERRORREPORT:QUEUE
csc -target:library -debug -pdb:IronSightEngine.pdb -out:IronSightEngine.dll *.cs
copy IronSightEngine_wrap.dll K:\C#\SwigTest2\SwigTest2\bin\Debug\
copy IronSightEngine_wrap.pdb K:\C#\SwigTest2\SwigTest2\bin\Debug\