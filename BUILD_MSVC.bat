@echo off
setlocal enabledelayedexpansion

:: Use vswhere.exe to find the Visual Studio installation directory
for /f "tokens=*" %%i in ('vswhere.exe -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do (
    set "msbuild_path=%%i"
    goto :found
)

:: If msbuild.exe is not found, display an error message
echo "msbuild.exe not found!"
exit /b 1

:found
:: Output the path to msbuild.exe
echo Found msbuild.exe at: %msbuild_path%

"%msbuild_path%" Coursework\Coursework.sln
pause


