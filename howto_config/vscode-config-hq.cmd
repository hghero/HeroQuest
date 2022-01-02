@ECHO OFF

rem ----------- cygwin -----------
set PATH=C:\cygwin64\bin;%PATH%
set CYGWIN=nodosfilewarning

rem ----------- Qt ------------
set QT_PATH=E:/Programmieren/Qt/6.2.2/msvc2019_64
set QT_PATH_BACKSLASH=E:\Programmieren\Qt\6.2.2\msvc2019_64
set PATH=%PATH%;%QT_PATH_BACKSLASH%\bin

rem ----------- MS compiler -----------
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

rem ----------- alias -----------
doskey vscode="E:\Programmieren\Microsoft VS Code\code.exe"
doskey ls=dir /W

rem ----------- debug -----------
set _NT_SYMBOL_PATH=%_NT_SYMBOL_PATH%;"E:\Programmieren\HeroQuest\execute\pdb";%QT_PATH_BACKSLASH%\bin

rem ----------- Windows -----------
set WINDOWS_UCRT_INCLUDE="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt"
set WINDOWS_UM_INCLUDE="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um"
set WINDOWS_SHARED_INCLUDE="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared"
set WINDOWS_UCRT_LIBPATH="C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\ucrt\x64"
set WINDOWS_UM_LIBPATH="C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64"

rem ----------- VS Code -----------
set WORKSPACE_DIR=e:/Programmieren/HeroQuest
set TARGET_MODE=DEBUG
set EXECUTE_DIR=execute
set RELEASE_DIR=release
cd e:\Programmieren\HeroQuest
e:
@ECHO ON
