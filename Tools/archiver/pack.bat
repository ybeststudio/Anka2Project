@echo off
rem Anka2 Pack Studio - komut satiri
rem   pack.bat plan                 tum pack'lerin durum raporu
rem   pack.bat build root etc       secili pack'leri Client\pack'e yaz
rem   pack.bat build --all          hepsi
rem   pack.bat verify               mevcut pack'leri dogrula
rem   pack.bat restore --dry        kaynakta silinmis dosyalari raporla (--dry olmadan geri yazar)
chcp 65001 >nul
set PYTHONUTF8=1
cd /d "%~dp0"
python -m pack_studio %*
