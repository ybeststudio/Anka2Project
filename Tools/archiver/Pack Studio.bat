@echo off
rem Anka2 Pack Studio (arayuz). Python 3.11+ gerekir; eksik paketler ilk calistirmada kurulur.
setlocal
cd /d "%~dp0"
set PYTHONUTF8=1

where python >nul 2>nul
if errorlevel 1 (
    echo [HATA] Python bulunamadi. Python 3.11+ kurup PATH'e ekleyin: https://www.python.org/downloads/
    pause
    exit /b 1
)

python -c "import sys; sys.exit(0 if sys.version_info >= (3, 11) else 1)"
if errorlevel 1 (
    echo [HATA] Python 3.11 veya ustu gerekli. Mevcut surum:
    python --version
    pause
    exit /b 1
)

python -c "import PyQt6.QtWidgets, lz4.block, Crypto.Cipher.AES, xxhash" >nul 2>nul
if errorlevel 1 (
    echo Eksik paketler kuruluyor: pack_studio\requirements.txt
    python -m pip install -r pack_studio\requirements.txt
    if errorlevel 1 (
        echo [HATA] Paket kurulumu basarisiz.
        pause
        exit /b 1
    )
)

where pythonw >nul 2>nul
if errorlevel 1 (
    python -m pack_studio
) else (
    start "" pythonw -m pack_studio
)
endlocal
