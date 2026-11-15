@echo off
chcp 65001 >nul
title Demo ao Vivo - Monitoramento de Carga (STM32MP1 + UDP + GUI)
cd /d "%~dp0.."

echo ============================================================================
echo   DEMONSTRACAO AO VIVO - MONITORAMENTO DE CARGA (SE/3 - IME)
echo ============================================================================

if not exist app_sensor_demo.exe (
    echo [1/3] Compilando aplicacao C++17 (IrObstacleSensor + UdpTransmitter)...
    g++ -Wall -Wextra -std=c++17 -Iinclude -O2 src/IrObstacleSensor.cpp src/UdpTransmitter.cpp src/main.cpp -o app_sensor_demo.exe -lws2_32
)

echo [2/3] Iniciando simulador dinamico do canal IIO (alternando tampa fechada/aberta a cada 5s)...
start "Simulador IIO HW-201" cmd /c "python scripts/mock_sensor.py mock_sensor_raw.txt 5"

echo [3/3] Iniciando Interface Grafica Python (Porta UDP 5000) e Transmissor C++17...
start "Dashboard Supervisorio Python (UDP 5000)" cmd /c "python gui/gui_dashboard.py"
timeout /t 2 /nobreak >nul

echo.
echo Transmitindo pacotes UDP em tempo real para 127.0.0.1:5000 (Pressione Ctrl+C para encerrar)...
echo.
.\app_sensor_demo.exe mock_sensor_raw.txt 0 127.0.0.1 5000
