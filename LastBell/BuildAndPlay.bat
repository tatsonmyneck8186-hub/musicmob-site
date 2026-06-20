@echo off
REM ===========================================================================
REM  LAST BELL - one-click build + launch helper (Windows, Unreal Engine 5.7)
REM
REM  Double-click this file. It locates UE 5.7, generates the Visual Studio
REM  project files, compiles the LastBell editor module, and opens the editor
REM  with the project. Then: File > New Level > Empty Level, and press Play.
REM
REM  If your engine is installed somewhere other than the default Epic path,
REM  set UE_ROOT below to your "...\UE_5.7" folder.
REM ===========================================================================
setlocal enabledelayedexpansion

set "PROJECT=%~dp0LastBell.uproject"

REM --- 1. Locate Unreal Engine 5.7 -------------------------------------------
set "UE_ROOT="
if exist "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" (
    set "UE_ROOT=C:\Program Files\Epic Games\UE_5.7"
)
REM (Set this manually if the auto-detect above fails, e.g.)
REM set "UE_ROOT=D:\Epic\UE_5.7"

if "%UE_ROOT%"=="" (
    echo.
    echo [ERROR] Could not find Unreal Engine 5.7 automatically.
    echo         Open this .bat in a text editor and set UE_ROOT to your
    echo         Unreal Engine 5.7 folder ^(the one containing \Engine^).
    echo.
    pause
    exit /b 1
)

echo Using engine: %UE_ROOT%
echo Project:      %PROJECT%
echo.

set "BUILD_BAT=%UE_ROOT%\Engine\Build\BatchFiles\Build.bat"

REM --- 2. Generate Visual Studio project files (optional, for IDE use) --------
echo [1/3] Generating project files...
"%UE_ROOT%\Engine\Build\BatchFiles\GenerateProjectFiles.bat" -project="%PROJECT%" -game -engine >nul 2>&1

REM --- 3. Build the editor target --------------------------------------------
echo [2/3] Building LastBellEditor (Development, Win64)...
call "%BUILD_BAT%" LastBellEditor Win64 Development -Project="%PROJECT%" -WaitMutex
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed. Scroll up for the first 'error C####:' line.
    echo         Paste that line ^(and file:line^) back to Claude to fix.
    echo.
    pause
    exit /b 1
)

REM --- 4. Launch the editor --------------------------------------------------
echo [3/3] Launching editor...
start "" "%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" "%PROJECT%"

echo.
echo Done. In the editor: File ^> New Level ^> Empty Level, then press Play.
endlocal
