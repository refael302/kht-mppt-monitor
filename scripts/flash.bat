@echo off
setlocal EnableExtensions
cd /d "%~dp0.."
chcp 65001 >nul

if not exist "platformio.ini" (
  echo platformio.ini was not found next to the scripts folder.
  pause
  exit /b 1
)

where python >nul 2>&1
if errorlevel 1 (
  echo Python was not found. Install Python 3 and enable Add to PATH.
  pause
  exit /b 1
)

python -c "import platformio" >nul 2>&1
if errorlevel 1 (
  echo Installing PlatformIO. The first run can take several minutes.
  python -m pip install platformio
  if errorlevel 1 (
    echo PlatformIO installation failed.
    pause
    exit /b 1
  )
)

echo.
echo Serial ports:
python -m platformio device list
echo.

if "%~1"=="" (
  echo Flashing the ESP32. Close any serial monitor that holds the COM port.
  python -m platformio run -t upload
) else (
  echo Flashing the ESP32 on %~1.
  python -m platformio run -t upload --upload-port %~1
)
set "ERR=%ERRORLEVEL%"

echo.
if not "%ERR%"=="0" (
  echo Flash failed. Pass the port if auto-detect picked the wrong one:
  echo   scripts\flash.bat COM5
) else (
  echo Flash finished. Join Wi-Fi KHT-MPPT and open http://192.168.4.1
)
pause
exit /b %ERR%
