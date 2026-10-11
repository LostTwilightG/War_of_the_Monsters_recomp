@echo off
rem Compila o visualizador nativo com MSVC (BuildTools 2022). Precisa do raylib ja compilado:
rem   set RAYLIB_DIR=<pasta com include\raylib.h e raylib.lib>   (padrao: o build do clone de teste do runtime)
setlocal
if "%RAYLIB_DIR%"=="" set "RAYLIB_DIR=C:\Users\TwistZero\wotm-recomp-win\build\_deps\raylib-build\raylib"
set "ProgramFiles(x86)=C:\Program Files (x86)"
set "PATH=C:\Program Files (x86)\Microsoft Visual Studio\Installer;%PATH%"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if not exist "%~dp0..\build" mkdir "%~dp0..\build"
cl /nologo /std:c++17 /O2 /EHsc /MD /I"%RAYLIB_DIR%\include" "%~dp0viewer.cpp" /Fo"%~dp0..\build\viewer.obj" /Fe"%~dp0..\build\viewer.exe" /link /LIBPATH:"%RAYLIB_DIR%" raylib.lib winmm.lib gdi32.lib user32.lib shell32.lib opengl32.lib
endlocal
