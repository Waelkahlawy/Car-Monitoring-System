@echo off
REM Script to create MQTT password file (Windows)
REM Usage: create_password.bat <username> <password>

if "%1"=="" (
    echo Usage: %0 ^<username^> ^<password^>
    echo Example: %0 mqtt_user mypassword123
    echo.
    echo Note: You need mosquitto installed or use Docker to create the password file
    echo.
    echo Alternative: Use Docker to create password file:
    echo   docker run -it --rm -v "%CD%\docker\mosquitto\config:/mosquitto/config" eclipse-mosquitto:2.0 mosquitto_passwd -c -b /mosquitto/config/passwd %1 %2
    exit /b 1
)

set USERNAME=%1
set PASSWORD=%2

echo Creating MQTT password file...
echo Username: %USERNAME%

REM Check if Docker is available
docker --version >nul 2>&1
if %errorlevel% equ 0 (
    echo Using Docker to create password file...
    docker run -it --rm -v "%CD%\docker\mosquitto\config:/mosquitto/config" eclipse-mosquitto:2.0 mosquitto_passwd -c -b /mosquitto/config/passwd %USERNAME% %PASSWORD%
    
    if %errorlevel% equ 0 (
        echo.
        echo ✓ Password file created successfully!
        echo   Username: %USERNAME%
        echo   Password file: docker\mosquitto\config\passwd
        echo.
        echo Next steps:
        echo 1. Update code.py with: MQTT_USERNAME = '%USERNAME%', MQTT_PASSWORD = '%PASSWORD%'
        echo 2. Update docker\telegraf\telegraf.conf with the same credentials
        echo 3. Restart containers: docker-compose restart mosquitto telegraf
    ) else (
        echo ✗ Failed to create password file
        exit /b 1
    )
) else (
    echo Error: Docker not found. Please install Docker Desktop or use the manual method.
    echo.
    echo Manual method:
    echo 1. Install Mosquitto from https://mosquitto.org/download/
    echo 2. Run: mosquitto_passwd -c -b docker\mosquitto\config\passwd %USERNAME% %PASSWORD%
    exit /b 1
)

pause

