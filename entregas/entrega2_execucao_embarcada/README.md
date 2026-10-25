# Entrega 2 — Programa Funcional no Kit STM32MP157D-DK1

**Grupo:** Joao Victor Pariz, Mateus Roma, Daniel Coimbra

## Arquivos desta Entrega
* [`Makefile`](Makefile): Regras de compilacao nativa (`make`) e compilacao cruzada ARM (`make cross`).
* [`cross_build.sh`](cross_build.sh): Script automatizado de cross-compilacao com a toolchain `arm-buildroot-linux-gnueabihf`.
* [`deploy_to_board.sh`](deploy_to_board.sh): Script de transferencia via `scp` e configuracao de permissao na placa (`192.168.42.2`).

## Instrucoes de Compilacao e Execucao no Kit
```bash
# 1. Compilar para ARM Cortex-A7:
bash cross_build.sh

# 2. Enviar para a placa via SCP:
scp app_sensor_arm root@192.168.42.2:/home/root/

# 3. Executar via SSH ou Serial (root / root):
./app_sensor_arm /sys/bus/iio/devices/iio:device0/in_voltage0_raw
```
