del *.cs *.c *.cxx *.obj IronSightEngine.dll IronSightEngine_wrap.dll *.exp *.ilk *.lib *.pdb *.idb
swig -csharp -c++ -D_WIN32 -dllimport IronSightEngine_wrap ISOpenGL.i
REM copy *.cs K:\C#\SwigTest\SwigTest\
cl -c ISOpenGL_wrap.cxx -I../../include/ /Zi /nologo /W3 /WX- /O2 /Oi /Oy- /GL /D "_WINDLL" /D "_MBCS" /Gm- /EHsc /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Fp"Release\IronSightEngine_wrap.pch" /Gd /analyze- /errorReport:queue /MD
link ISOpenGL_wrap.obj /OUT:"IronSightEngine_wrap.dll" /NOLOGO /DLL "K:\C++\IronSightEngine\lib\x86\ISEngine.lib" "K:\C++\IronSightEngine\lib\x86\ISMath.lib" "K:\C++\IronSightEngine\dll\x86\ISOpenGL.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /OPT:REF /OPT:ICF /LTCG /TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X86 /ERRORREPORT:QUEUE
csc -target:library -out:IronSightEngine.dll *.cs
REM copy *.dll K:\C#\SwigTest\SwigTest\bin\Release\