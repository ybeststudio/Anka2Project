@echo off
set "ARCHIVER_DIR=%~dp0"
set "PATH=%ARCHIVER_DIR%tools\Python27;%ARCHIVER_DIR%tools;%PATH%"
if "%~1"=="" goto build_with_param
"%ARCHIVER_DIR%tools\Archiver.exe" make_xml/%~1_create.xml
"%ARCHIVER_DIR%tools\Archiver.exe" xml/%~1_create.xml
echo "Done"
goto end
:build_with_param
set /p id="Enter File to make: "
"%ARCHIVER_DIR%tools\Archiver.exe" make_xml/%id%_create.xml
"%ARCHIVER_DIR%tools\Archiver.exe" xml/%id%_create.xml > output.txt
goto build_with_param
pause
:end
echo "Finished Execution"