del ..\*.cs ..\*.cxx ..\*.obj ..\*.dll ..\*.exp ..\*.ilk ..\*.lib ..\*.pdb ..\*.idb
swig -csharp -c++ -D_WIN32 -DSWIG_MATH -DSWIG_DIRECT3D -DSWIG_PBRT -dllimport IronSightEngine_wrap ../IronSightEngine.i
REM copy ../*.cs K:\C#\SwigTest\SwigTest\
cl -c ../IronSightEngine_wrap.cxx -I../../include/ -I"D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include" /ZI /nologo /W3 /WX- /Od /Oy- /D "_WINDLL" /D "_MBCS" /Gm /EHsc /RTC1 /GS /fp:precise /Zc:wchar_t /Zc:forScope /Fd"vc100.pdb" /Gd /analyze- /errorReport:queue /MDd -Fo../IronSightEngine_wrap.obj
link ../IronSightEngine_wrap.obj /OUT:"../IronSightEngine_wrap.dll" /NOLOGO /DLL "K:\C++\IronSightEngine\lib\x86\ISEngine_d.lib" "K:\C++\IronSightEngine\lib\x86\ISMath_d.lib" "K:\C++\IronSightEngine\dll\x86\ISDirect3D_d.lib" "K:\C++\IronSightEngine\dll\x86\ISPbrt_d.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /DEBUG /PDB:"../IronSightEngine_wrap.pdb" /TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X86 /ERRORREPORT:QUEUE
csc -target:library -debug -pdb:../IronSightEngine.pdb -out:../IronSightEngine.dll ..\*.cs
copy ..\*.dll K:\C#\IronSightArchitect\IronSightArchitect\bin\Debug
copy ..\*.pdb K:\C#\IronSightArchitect\IronSightArchitect\bin\Debug