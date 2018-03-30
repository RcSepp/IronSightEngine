del ..\*.cs ..\*.cxx ..\*.obj ..\*.dll ..\*.exp ..\*.ilk ..\*.lib ..\*.pdb ..\*.idb
swig -csharp -c++ -D_WIN32 -DSWIG_MATH -DSWIG_DIRECT3D -DSWIG_PBRT -dllimport IronSightEngine_wrap ../IronSightEngine.i
REM copy ../*.cs K:\C#\SwigTest\SwigTest\
cl -c ../IronSightEngine_wrap.cxx -I../../include/ -I"D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include" /Zi /nologo /W3 /WX- /O2 /Oi /Oy- /GL /D "_WINDLL" /D "_MBCS" /Gm- /EHsc /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Gd /analyze- /errorReport:queue /MD -Fo../IronSightEngine_wrap.obj
link ../IronSightEngine_wrap.obj /OUT:"../IronSightEngine_wrap.dll" /NOLOGO /DLL "K:\C++\IronSightEngine\lib\x86\ISEngine.lib" "K:\C++\IronSightEngine\lib\x86\ISMath.lib" "K:\C++\IronSightEngine\dll\x86\ISDirect3D.lib" "K:\C++\IronSightEngine\dll\x86\ISPbrt.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /OPT:REF /OPT:ICF /LTCG /TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X86 /ERRORREPORT:QUEUE
csc -target:library -debug -pdb:../IronSightEngine.pdb -out:../IronSightEngine.dll ..\*.cs
copy ..\*.dll K:\C#\IronSightArchitect\IronSightArchitect\bin\Debug
copy ..\*.pdb K:\C#\IronSightArchitect\IronSightArchitect\bin\Debug