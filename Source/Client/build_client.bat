@echo off
REM ============================================================================
REM >  Anka2 Client Build System
REM >  Professional Command-Line Build Tool
REM >  Version: 1.0
REM ============================================================================

setlocal enabledelayedexpansion

REM >> RENK KODLARI <<
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "MAGENTA=[95m"
set "CYAN=[96m"
set "WHITE=[97m"
set "BOLD=[1m"
set "RESET=[0m"

REM >> AYARLAR <<
set "SOLUTION_DIR=%~dp0"
set "SOLUTION_FILE=%SOLUTION_DIR%client.sln"
set "BUILD_LOG_DIR=%SOLUTION_DIR%build_logs"
set "DEFAULT_CONFIG=Release"
set "DEFAULT_PLATFORM=x64"

REM >> KULLANIM YARDIMI <<
if "%1"=="" goto :usage
if /i "%1"=="help" goto :usage
if /i "%1"=="/?" goto :usage

REM >> PARAMETRE AYIRMA <<
set "COMMAND=%1"
set "CONFIG=%2"
set "PLATFORM=%3"

if "%CONFIG%"=="" set "CONFIG=%DEFAULT_CONFIG%"
if "%PLATFORM%"=="" set "PLATFORM=%DEFAULT_PLATFORM%"
REM UserInterface TargetName: Win32 -> Metin2<Config>.exe, x64 -> Metin2<Config>_x64.exe
set "EXE_SUFFIX="
if /i "%PLATFORM%"=="x64" set "EXE_SUFFIX=_x64"

REM >> KONFIGURASYON KONTROLU <<
if /i not "%CONFIG%"=="Debug" (
    if /i not "%CONFIG%"=="Release" (
        if /i not "%CONFIG%"=="Distribute" (
            echo %RED%[HATA]%RESET% Gecersiz konfigurasyon: %CONFIG%
            echo Gecerli konfigurasyonlar: Debug, Release, Distribute
            exit /b 1
        )
    )
)

REM >> LOG DIZINI OLUSTURMA <<
if not exist "%BUILD_LOG_DIR%" mkdir "%BUILD_LOG_DIR%"

REM >> TIMESTAMP <<
for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set "datetime=%%I"
set "TIMESTAMP=%datetime:~0,8%_%datetime:~8,6%"
set "LOG_FILE=%BUILD_LOG_DIR%\build_%CONFIG%_%TIMESTAMP%.log"

REM >> MSBUILD PATH BULMA <<
set "MSBUILD_PATH="
if exist "%ProgramFiles%\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles%\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
)
if not defined MSBUILD_PATH if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
)
if not defined MSBUILD_PATH if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
)
if not defined MSBUILD_PATH if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
)
if not defined MSBUILD_PATH if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
)
if not defined MSBUILD_PATH if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
)
if not defined MSBUILD_PATH if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
)

if "%MSBUILD_PATH%"=="" (
    echo %RED%[HATA]%RESET% MSBuild bulunamadi!
    echo Visual Studio 2026 - v145 toolset - yuklu oldugundan emin olun.
    exit /b 1
)

REM >> KOMUT YONLENDIRME <<
if /i "%COMMAND%"=="clean" goto :clean
if /i "%COMMAND%"=="build" goto :build
if /i "%COMMAND%"=="rebuild" goto :rebuild
if /i "%COMMAND%"=="info" goto :info

echo %RED%[HATA]%RESET% Gecersiz komut: %COMMAND%
goto :usage

REM ============================================================================
REM >> TEMIZLEME <<
REM ============================================================================
:clean
echo.
echo %CYAN%============================================================================%RESET%
echo %BOLD%%CYAN%                    CLIENT CLEAN: %CONFIG% %PLATFORM%%RESET%
echo %CYAN%============================================================================%RESET%
echo.

"%MSBUILD_PATH%" "%SOLUTION_FILE%" /t:Clean /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /v:minimal /nologo

if errorlevel 1 (
    echo.
    echo %RED%[HATA]%RESET% Temizleme basarisiz oldu!
    exit /b 1
)

echo.
echo %GREEN%[BASARILI]%RESET% Temizleme tamamlandi!
goto :end

REM ============================================================================
REM >> BUILD <<
REM ============================================================================
:build
echo.
echo %CYAN%============================================================================%RESET%
echo %BOLD%%CYAN%                    CLIENT BUILD: %CONFIG% %PLATFORM%%RESET%
echo %CYAN%============================================================================%RESET%
echo.
echo %YELLOW%[BILGI]%RESET% Solution: %SOLUTION_FILE%
echo %YELLOW%[BILGI]%RESET% Log Dosyasi: %LOG_FILE%
echo %YELLOW%[BILGI]%RESET% MSBuild: %MSBUILD_PATH%
echo.

set "START_TIME=%time%"
"%MSBUILD_PATH%" "%SOLUTION_FILE%" /t:Build /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /v:normal /m /fl /flp:logfile=%LOG_FILE%;verbosity=detailed /nologo

if errorlevel 1 (
    echo.
    echo %RED%[HATA]%RESET% Build basarisiz oldu!
    echo %YELLOW%[BILGI]%RESET% Detayli log: %LOG_FILE%
    exit /b 1
)

set "END_TIME=%time%"
echo.
echo %GREEN%============================================================================%RESET%
echo %BOLD%%GREEN%                    BUILD BASARILI!%RESET%
echo %GREEN%============================================================================%RESET%
echo.
echo %GREEN%[BASARILI]%RESET% Build tamamlandi!
echo %YELLOW%[BILGI]%RESET% Baslangic: %START_TIME%
echo %YELLOW%[BILGI]%RESET% Bitis: %END_TIME%
echo %YELLOW%[BILGI]%RESET% Log Dosyasi: %LOG_FILE%
echo %YELLOW%[BILGI]%RESET% Output: %SOLUTION_DIR%binary\Metin2%CONFIG%%EXE_SUFFIX%.exe
echo.
goto :end

REM ============================================================================
REM >> REBUILD <<
REM ============================================================================
:rebuild
echo.
echo %CYAN%============================================================================%RESET%
echo %BOLD%%CYAN%                  CLIENT REBUILD: %CONFIG% %PLATFORM%%RESET%
echo %CYAN%============================================================================%RESET%
echo.

echo %YELLOW%[ADIM 1/2]%RESET% Temizleme yapiliyor...
"%MSBUILD_PATH%" "%SOLUTION_FILE%" /t:Clean /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /v:minimal /nologo

if errorlevel 1 (
    echo.
    echo %RED%[HATA]%RESET% Temizleme basarisiz oldu!
    exit /b 1
)

echo %GREEN%[ADIM 1/2]%RESET% Temizleme tamamlandi!
echo.
echo %YELLOW%[ADIM 2/2]%RESET% Build yapiliyor...
echo.

set "START_TIME=%time%"
"%MSBUILD_PATH%" "%SOLUTION_FILE%" /t:Build /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /v:normal /m /fl /flp:logfile=%LOG_FILE%;verbosity=detailed /nologo

if errorlevel 1 (
    echo.
    echo %RED%[HATA]%RESET% Build basarisiz oldu!
    echo %YELLOW%[BILGI]%RESET% Detayli log: %LOG_FILE%
    exit /b 1
)

set "END_TIME=%time%"
echo.
echo %GREEN%============================================================================%RESET%
echo %BOLD%%GREEN%                    REBUILD BASARILI!%RESET%
echo %GREEN%============================================================================%RESET%
echo.
echo %GREEN%[BASARILI]%RESET% Rebuild tamamlandi!
echo %YELLOW%[BILGI]%RESET% Baslangic: %START_TIME%
echo %YELLOW%[BILGI]%RESET% Bitis: %END_TIME%
echo %YELLOW%[BILGI]%RESET% Log Dosyasi: %LOG_FILE%
echo %YELLOW%[BILGI]%RESET% Output: %SOLUTION_DIR%binary\Metin2%CONFIG%%EXE_SUFFIX%.exe
echo.
goto :end

REM ============================================================================
REM >> INFO <<
REM ============================================================================
:info
echo.
echo %CYAN%============================================================================%RESET%
echo %BOLD%%CYAN%                    CLIENT BUILD SYSTEM INFO%RESET%
echo %CYAN%============================================================================%RESET%
echo.
echo %WHITE%Solution Dosyasi:%RESET% %SOLUTION_FILE%
echo %WHITE%MSBuild Path:%RESET% %MSBUILD_PATH%
echo %WHITE%Log Dizini:%RESET% %BUILD_LOG_DIR%
echo %WHITE%Varsayilan Konfigurasyon:%RESET% %DEFAULT_CONFIG%
echo %WHITE%Varsayilan Platform:%RESET% %DEFAULT_PLATFORM%
echo.
if exist "%SOLUTION_DIR%binary\Metin2Debug_x64.exe" (
    echo %GREEN%[MEVCUT]%RESET% Debug x64 build bulundu: %SOLUTION_DIR%binary\Metin2Debug_x64.exe
) else (
    echo %RED%[YOK]%RESET% Debug x64 build bulunamadi
)
if exist "%SOLUTION_DIR%binary\Metin2Release_x64.exe" (
    echo %GREEN%[MEVCUT]%RESET% Release x64 build bulundu: %SOLUTION_DIR%binary\Metin2Release_x64.exe
) else (
    echo %RED%[YOK]%RESET% Release x64 build bulunamadi
)
echo.
goto :end

REM ============================================================================
REM >> KULLANIM <<
REM ============================================================================
:usage
echo.
echo %CYAN%============================================================================%RESET%
echo %BOLD%%CYAN%                    ANKA2 CLIENT BUILD SYSTEM%RESET%
echo %CYAN%============================================================================%RESET%
echo.
echo %YELLOW%KULLANIM:%RESET%
echo   %BOLD%build_client.bat%RESET% [komut] [konfigurasyon] [platform]
echo.
echo %YELLOW%KOMUTLAR:%RESET%
echo   %GREEN%build%RESET%      - Client'i derler
echo   %GREEN%rebuild%RESET%    - Once temizler, sonra derler
echo   %GREEN%clean%RESET%      - Derleme dosyalarini temizler
echo   %GREEN%info%RESET%       - Build sistem bilgilerini gosterir
echo   %GREEN%help%RESET%       - Bu yardim mesajini gosterir
echo.
echo %YELLOW%KONFIGURASYONLAR:%RESET%
echo   %GREEN%Debug%RESET%      - Debug build (varsayilan)
echo   %GREEN%Release%RESET%    - Release build
echo.
echo %YELLOW%PLATFORM:%RESET%
echo   %GREEN%x64%RESET%        - 64-bit build (varsayilan)
echo   %GREEN%Win32%RESET%      - 32-bit build
echo.
echo %YELLOW%ORNEKLER:%RESET%
echo   %BOLD%build_client.bat build%RESET%                - Release build yapar
echo   %BOLD%build_client.bat build Debug%RESET%          - Debug build yapar
echo   %BOLD%build_client.bat rebuild Release%RESET%      - Release rebuild yapar
echo   %BOLD%build_client.bat clean Debug%RESET%          - Debug temizler
echo   %BOLD%build_client.bat info%RESET%                 - Sistem bilgilerini gosterir
echo.
echo %CYAN%============================================================================%RESET%
echo.
goto :end

REM ============================================================================
REM >> CIKIS <<
REM ============================================================================
:end
endlocal

