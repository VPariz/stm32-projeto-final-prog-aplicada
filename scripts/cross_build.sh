#!/bin/bash
# Joao Victor Pariz
# Projeto Final - Sensor HW-201
# SE/3 - Eletronica - Programacao Aplicada
# Script de automacao de compilacao cruzada para a placa STM32MP157D-DK1 (Arm Cortex-A7)

set -e

echo "=========================================================="
echo " SE/3 - IME | Cross-Compilacao STM32MP157D-DK1"
echo "=========================================================="

# Procura a toolchain oficial nos locais padrao
TOOLCHAIN_PATHS=(
    "/opt/arm-buildroot-linux-gnueabihf_sdk-DK2/bin"
    "$HOME/toolchain/arm-buildroot-linux-gnueabihf_sdk-DK2/bin"
    "/usr/local/arm-buildroot-linux-gnueabihf_sdk-DK2/bin"
)

FOUND=0
for P in "${TOOLCHAIN_PATHS[@]}"; do
    if [ -d "$P" ]; then
        export PATH="$P:$PATH"
        FOUND=1
        echo "[INFO] Toolchain localizada em: $P"
        break
    fi
done

CROSS_CXX="arm-buildroot-linux-gnueabihf-g++"

if ! command -v $CROSS_CXX &> /dev/null; then
    echo "[AVISO] Compilador '$CROSS_CXX' nao encontrado no PATH."
    echo "Tentando compilador cruzado generico 'arm-linux-gnueabihf-g++'..."
    CROSS_CXX="arm-linux-gnueabihf-g++"
    if ! command -v $CROSS_CXX &> /dev/null; then
        echo "[ERRO] Nenhum compilador cruzado ARM encontrado."
        echo "Dica: Instale a toolchain fornecida ou instale via apt no Ubuntu:"
        echo "      sudo apt update && sudo apt install -y g++-arm-linux-gnueabihf"
        exit 1
    fi
fi

echo "[INFO] Usando compilador: $(command -v $CROSS_CXX)"
$CROSS_CXX --version | head -n 1

echo "[BUILD] Compilando aplicacao para o target ARM..."
$CROSS_CXX -Wall -Wextra -std=c++17 -O2 \
    -Iinclude \
    src/IrObstacleSensor.cpp \
    src/main.cpp \
    -o app_sensor_arm

echo "[SUCESSO] Binario ARM gerado: app_sensor_arm"
file app_sensor_arm
ls -lh app_sensor_arm
echo "=========================================================="
