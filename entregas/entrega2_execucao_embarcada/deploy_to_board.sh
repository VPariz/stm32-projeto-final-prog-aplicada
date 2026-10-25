#!/bin/bash
# Joao Victor Pariz
# Projeto Final - Sensor HW-201
# SE/3 - Eletronica - Programacao Aplicada
# Script para transferir e testar o executavel diretamente na placa STM32MP157D-DK1

BOARD_IP="${1:-192.168.42.2}"
BOARD_USER="root"
REMOTE_DIR="/home/root"
BINARY="app_sensor_arm"

echo "=========================================================="
echo " SE/3 - IME | Deploy de Binario na Placa STM32MP157D-DK1"
echo " Alvo: $BOARD_USER@$BOARD_IP:$REMOTE_DIR"
echo "=========================================================="

if [ ! -f "$BINARY" ]; then
    echo "[ERRO] Arquivo '$BINARY' nao encontrado na pasta atual."
    echo "Execute primeiro o script 'scripts/cross_build.sh'."
    exit 1
fi

echo "[1/3] Testando conectividade com a placa ($BOARD_IP)..."
if ! ping -c 1 -W 2 "$BOARD_IP" > /dev/null 2>&1; then
    echo "[AVISO] Placa nao respondeu ao ping. Verifique se o cabo Ethernet esta conectado"
    echo "        e se o IP do seu computador na interface cabeada e '192.168.42.10'."
fi

echo "[2/3] Enviando binario '$BINARY' via SCP..."
scp -o ConnectTimeout=5 "$BINARY" "$BOARD_USER@$BOARD_IP:$REMOTE_DIR/"

echo "[3/3] Aplicando permissao de execucao no target..."
ssh -o ConnectTimeout=5 "$BOARD_USER@$BOARD_IP" "chmod +x $REMOTE_DIR/$BINARY && ls -lh $REMOTE_DIR/$BINARY"

echo "=========================================================="
echo "[SUCESSO] Deploy concluido com sucesso!"
echo "Para executar na placa:"
echo "  ssh $BOARD_USER@$BOARD_IP"
echo "  cd $REMOTE_DIR"
echo "  ./$BINARY /sys/bus/iio/devices/iio:device0/in_voltage0_raw 192.168.42.10 5000"
echo "=========================================================="
