@echo off
setlocal

set SHADERCROSS=%~dp0Shadercross\shadercross.exe
set INPUT=%~1
set OUTPUT=%~2

if not exist "%OUTPUT%" mkdir "%OUTPUT%"

echo Compiling shaders...

REM =========================
REM Vertex Shaders
REM =========================

for %%f in ("%INPUT%\*.vert.hlsl") do (
    echo Compiling %%~nxf

    "%SHADERCROSS%" "%%f" -s HLSL -d SPIRV -t vertex -e main -o "%OUTPUT%\%%~nf.spv"

    "%SHADERCROSS%" "%%f" -s HLSL -d DXIL -t vertex -e main -o "%OUTPUT%\%%~nf.dxil"
)

REM =========================
REM Fragment Shaders
REM =========================

for %%f in ("%INPUT%\*.frag.hlsl") do (
    echo Compiling %%~nxf

    "%SHADERCROSS%" "%%f" -s HLSL -d SPIRV -t fragment -e main -o "%OUTPUT%\%%~nf.spv"

    "%SHADERCROSS%" "%%f" -s HLSL -d DXIL -t fragment -e main -o "%OUTPUT%\%%~nf.dxil"
)

echo Shader compilation complete.