@echo off
setlocal enabledelayedexpansion

echo ========================================
echo   GBFR Skill Editor - Deploy Tool
echo ========================================
echo.

REM --- Config (adjust if your paths differ) ---
set QT_DIR=G:\Softwares\Development_Softwares\Qt\6.9.3\mingw_64
set BUILD_DIR=build\Desktop_Qt_6_9_3_MinGW_64_bit-Release
set DIST_DIR=dist\GBFR_SkillEditor

echo Make sure you have built Release mode in Qt Creator first.
echo Build output folder: %BUILD_DIR%
echo.
pause

REM --- Step 1: create dist folder ---
echo [1/3] Creating dist folder...
if exist %DIST_DIR% rmdir /s /q %DIST_DIR%
mkdir %DIST_DIR%
echo OK

REM --- Step 2: copy exe + resources ---
echo [2/3] Copying exe and resources...
copy /Y %BUILD_DIR%\GBFR_SkillEditor.exe %DIST_DIR%\ >nul
copy /Y style.qss %DIST_DIR%\ >nul
copy /Y skill_id_map.json %DIST_DIR%\ >nul
copy /Y app_icon.ico %DIST_DIR%\ >nul
echo OK

REM --- Step 3: collect Qt DLLs ---
echo [3/3] Collecting Qt DLLs via windeployqt...
"%QT_DIR%\bin\windeployqt.exe" ^
    --release ^
    --no-translations ^
    --no-opengl-sw ^
    %DIST_DIR%\GBFR_SkillEditor.exe

if errorlevel 1 (
    echo windeployqt failed. Check QT_DIR path.
    pause
    exit /b 1
)

echo.
echo ========================================
echo   DONE!
echo   Output: %DIST_DIR%
echo   Size:   ~40 MB
echo   Copy this folder to any Windows PC
echo ========================================
echo.
echo Files:
dir /b %DIST_DIR%\*.exe %DIST_DIR%\*.dll %DIST_DIR%\*.qss 2>nul
echo.
pause
