@echo off
REM ============================================================================
REM >  Anka2 Client VS Files Cleaner
REM >  Professional Clean Tool for Debug/Release/Distribute Folders
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
set "VS_FILES_DIR=%~dp0vs_files"
set "CLEAN_ALL=0"
set "CLEAN_DEBUG=0"
set "CLEAN_RELEASE=0"
set "CLEAN_DISTRIBUTE=0"

REM >> KULLANIM YARDIMI <<
if "%1"=="" set "CLEAN_ALL=1"
if /i "%1"=="all" set "CLEAN_ALL=1"
if /i "%1"=="debug" set "CLEAN_DEBUG=1"
if /i "%1"=="release" set "CLEAN_RELEASE=1"
if /i "%1"=="distribute" set "CLEAN_DISTRIBUTE=1"
if /i "%1"=="help" goto :usage
if /i "%1"=="/?" goto :usage

if %CLEAN_ALL%==0 if %CLEAN_DEBUG%==0 if %CLEAN_RELEASE%==0 if %CLEAN_DISTRIBUTE%==0 set "CLEAN_ALL=1"
if %CLEAN_ALL%==1 (
    set "CLEAN_DEBUG=0"
    set "CLEAN_RELEASE=0"
    set "CLEAN_DISTRIBUTE=0"
)

REM >> KLASOR VARLIK KONTROLU <<
if not exist "%VS_FILES_DIR%" (
    echo %RED%[HATA]%RESET% vs_files klasoru bulunamadi: %VS_FILES_DIR%
    exit /b 1
)

REM >> TEMIZLENEN DOSYA SAYACI <<
set "TOTAL_DELETED=0"
set "TOTAL_FOLDERS=0"
set "TOTAL_SIZE=0"

REM >> BASLANGIC BILGILENDIRMESI <<
echo.
echo %CYAN%============================================================================%RESET%
echo %BOLD%%CYAN%                    VS FILES CLEANER%RESET%
echo %CYAN%============================================================================%RESET%
echo.
echo %YELLOW%[BILGI]%RESET% Hedef dizin: %VS_FILES_DIR%
echo.

REM >> TEMIZLENEKLER BELIRLEME <<
set "CLEAN_LIST="
if %CLEAN_ALL%==1 (
    set "CLEAN_LIST=Debug Release Distribute"
    echo %YELLOW%[HEDEF]%RESET% Tum klasorler temizlenecek: Debug, Release, Distribute
) else (
    if %CLEAN_DEBUG%==1 (
        set "CLEAN_LIST=%CLEAN_LIST% Debug"
        echo %YELLOW%[HEDEF]%RESET% Debug klasorleri temizlenecek
    )
    if %CLEAN_RELEASE%==1 (
        set "CLEAN_LIST=%CLEAN_LIST% Release"
        echo %YELLOW%[HEDEF]%RESET% Release klasorleri temizlenecek
    )
    if %CLEAN_DISTRIBUTE%==1 (
        set "CLEAN_LIST=%CLEAN_LIST% Distribute"
        echo %YELLOW%[HEDEF]%RESET% Distribute klasorleri temizlenecek
    )
)
echo.

REM >> ONAY ISTEME <<
echo %YELLOW%[UYARI]%RESET% Bu islem geri alinamaz!
echo.
set /p "CONFIRM=Devam etmek istiyor musunuz? (E/H): "
if /i not "!CONFIRM!"=="E" (
    echo.
    echo %YELLOW%[IPTAL]%RESET% Islem iptal edildi.
    exit /b 0
)
echo.

REM >> TEMIZLEME BASLATMA <<
set "START_TIME=%time%"

REM >> HER PROJE KLASORUNDE GEZ <<
for /d %%P in ("%VS_FILES_DIR%\*") do (
    set "PROJECT_DIR=%%P"
    set "PROJECT_NAME=%%~nP"
    
    if not "!PROJECT_NAME!"=="desktop.ini" (
        if %CLEAN_ALL%==1 (
            REM >> TUM KLASORLERI TEMIZLE <<
            set "CLEAN_FOLDER=!PROJECT_DIR!\Debug"
            if exist "!CLEAN_FOLDER!" (
                echo %CYAN%[TEMIZLENIYOR]%RESET% !PROJECT_NAME!\Debug
                rd /s /q "!CLEAN_FOLDER!" 2>nul
                if errorlevel 1 (
                    echo %RED%[HATA]%RESET% !PROJECT_NAME!\Debug silinemedi!
                ) else (
                    echo %GREEN%[SILINDI]%RESET% !PROJECT_NAME!\Debug
                    set /a TOTAL_FOLDERS+=1
                )
            )
            
            set "CLEAN_FOLDER=!PROJECT_DIR!\Release"
            if exist "!CLEAN_FOLDER!" (
                echo %CYAN%[TEMIZLENIYOR]%RESET% !PROJECT_NAME!\Release
                rd /s /q "!CLEAN_FOLDER!" 2>nul
                if errorlevel 1 (
                    echo %RED%[HATA]%RESET% !PROJECT_NAME!\Release silinemedi!
                ) else (
                    echo %GREEN%[SILINDI]%RESET% !PROJECT_NAME!\Release
                    set /a TOTAL_FOLDERS+=1
                )
            )
            
            set "CLEAN_FOLDER=!PROJECT_DIR!\Distribute"
            if exist "!CLEAN_FOLDER!" (
                echo %CYAN%[TEMIZLENIYOR]%RESET% !PROJECT_NAME!\Distribute
                rd /s /q "!CLEAN_FOLDER!" 2>nul
                if errorlevel 1 (
                    echo %RED%[HATA]%RESET% !PROJECT_NAME!\Distribute silinemedi!
                ) else (
                    echo %GREEN%[SILINDI]%RESET% !PROJECT_NAME!\Distribute
                    set /a TOTAL_FOLDERS+=1
                )
            )
        ) else (
            REM >> SECILI KLASORLERI TEMIZLE <<
            if %CLEAN_DEBUG%==1 (
                set "CLEAN_FOLDER=!PROJECT_DIR!\Debug"
                if exist "!CLEAN_FOLDER!" (
                    echo %CYAN%[TEMIZLENIYOR]%RESET% !PROJECT_NAME!\Debug
                    rd /s /q "!CLEAN_FOLDER!" 2>nul
                    if errorlevel 1 (
                        echo %RED%[HATA]%RESET% !PROJECT_NAME!\Debug silinemedi!
                    ) else (
                        echo %GREEN%[SILINDI]%RESET% !PROJECT_NAME!\Debug
                        set /a TOTAL_FOLDERS+=1
                    )
                )
            )
            
            if %CLEAN_RELEASE%==1 (
                set "CLEAN_FOLDER=!PROJECT_DIR!\Release"
                if exist "!CLEAN_FOLDER!" (
                    echo %CYAN%[TEMIZLENIYOR]%RESET% !PROJECT_NAME!\Release
                    rd /s /q "!CLEAN_FOLDER!" 2>nul
                    if errorlevel 1 (
                        echo %RED%[HATA]%RESET% !PROJECT_NAME!\Release silinemedi!
                    ) else (
                        echo %GREEN%[SILINDI]%RESET% !PROJECT_NAME!\Release
                        set /a TOTAL_FOLDERS+=1
                    )
                )
            )
            
            if %CLEAN_DISTRIBUTE%==1 (
                set "CLEAN_FOLDER=!PROJECT_DIR!\Distribute"
                if exist "!CLEAN_FOLDER!" (
                    echo %CYAN%[TEMIZLENIYOR]%RESET% !PROJECT_NAME!\Distribute
                    rd /s /q "!CLEAN_FOLDER!" 2>nul
                    if errorlevel 1 (
                        echo %RED%[HATA]%RESET% !PROJECT_NAME!\Distribute silinemedi!
                    ) else (
                        echo %GREEN%[SILINDI]%RESET% !PROJECT_NAME!\Distribute
                        set /a TOTAL_FOLDERS+=1
                    )
                )
            )
        )
    )
)

REM >> BITIS BILGILENDIRMESI <<
set "END_TIME=%time%"
echo.
echo %GREEN%============================================================================%RESET%
echo %BOLD%%GREEN%                    TEMIZLEME TAMAMLANDI!%RESET%
echo %GREEN%============================================================================%RESET%
echo.
echo %GREEN%[BASARILI]%RESET% Temizleme islemi tamamlandi!
echo %YELLOW%[BILGI]%RESET% Baslangic: %START_TIME%
echo %YELLOW%[BILGI]%RESET% Bitis: %END_TIME%
echo %YELLOW%[ISTATISTIK]%RESET% Temizlenen klasor sayisi: %TOTAL_FOLDERS%
echo.
goto :end

REM ============================================================================
REM >> KULLANIM <<
REM ============================================================================
:usage
echo.
echo %CYAN%============================================================================%RESET%
echo %BOLD%%CYAN%                    VS FILES CLEANER%RESET%
echo %CYAN%============================================================================%RESET%
echo.
echo %YELLOW%KULLANIM:%RESET%
echo   %BOLD%clear_vsfiles.bat%RESET% [secenek]
echo.
echo %YELLOW%SECENEKLER:%RESET%
echo   %GREEN%all%RESET%         - Tum klasorleri temizler (Debug, Release, Distribute) [varsayilan]
echo   %GREEN%debug%RESET%       - Sadece Debug klasorlerini temizler
echo   %GREEN%release%RESET%     - Sadece Release klasorlerini temizler
echo   %GREEN%distribute%RESET%  - Sadece Distribute klasorlerini temizler
echo   %GREEN%help%RESET%        - Bu yardim mesajini gosterir
echo.
echo %YELLOW%ORNEKLER:%RESET%
echo   %BOLD%clear_vsfiles.bat%RESET%              - Tum klasorleri temizler
echo   %BOLD%clear_vsfiles.bat all%RESET%          - Tum klasorleri temizler
echo   %BOLD%clear_vsfiles.bat debug%RESET%        - Sadece Debug klasorlerini temizler
echo   %BOLD%clear_vsfiles.bat release%RESET%      - Sadece Release klasorlerini temizler
echo   %BOLD%clear_vsfiles.bat distribute%RESET%   - Sadece Distribute klasorlerini temizler
echo.
echo %YELLOW%NOT:%RESET%
echo   - Bu islem geri alinamaz!
echo   - Temizlemeden once onay alinir
echo   - vs_files klasorundeki tum projelerin ilgili klasorleri temizlenir
echo.
echo %CYAN%============================================================================%RESET%
echo.
goto :end

REM ============================================================================
REM >> CIKIS <<
REM ============================================================================
:end
endlocal

