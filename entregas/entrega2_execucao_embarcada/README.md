# Entrega 2 — Programa Funcional no Kit de Desenvolvimento STM32MP157D-DK1

**Instituto Militar de Engenharia — IME**  
**Seção de Engenharia Elétrica — SE/3**  
**Disciplina:** Programação Aplicada à Engenharia Elétrica  
**Autor:** João Victor Tavares Pariz  
**Sensor:** Módulo de Obstáculo Infravermelho Reflexivo (**HW-201 / LM393**)  

---

## 📌 Resumo da Entrega 2
Programa funcional em C++17 Orientado a Objetos para execução em espaço de usuário Linux no kit **STM32MP157D-DK1**, realizando a leitura contínua dos dados do sensor através do subsistema **Linux IIO** (`/sys/bus/iio/devices/iio:device0/in_voltageX_raw`), conversão para tensão física em Volts ($0{,}00\text{ V}$ a $3{,}30\text{ V}$) e detecção de violação de carga.

### Arquivos Relacionados
* Código-fonte da classe e aplicação: [`../../include/IrObstacleSensor.hpp`](../../include/IrObstacleSensor.hpp), [`../../src/IrObstacleSensor.cpp`](../../src/IrObstacleSensor.cpp) e [`../../src/main.cpp`](../../src/main.cpp).
* [`Makefile`](Makefile): Automação de compilação nativa (`make`) e compilação cruzada ARM (`make cross`).
* [`cross_build.sh`](cross_build.sh): Script automatizado de compilação cruzada com a toolchain `arm-buildroot-linux-gnueabihf_sdk-DK2`.
* [`deploy_to_board.sh`](deploy_to_board.sh): Script de envio via `scp` e ajuste de permissão de execução na placa (`192.168.42.2`).

---

## 🚀 Passo a Passo de Compilação e Execução no Kit

### 1. Extrair e Configurar a Toolchain na Máquina Virtual Linux
```bash
tar -xvf arm-buildroot-linux-gnueabihf_sdk-DK2.tar.gz
export PATH=$PWD/arm-buildroot-linux-gnueabihf_sdk-DK2/bin:$PATH
```

### 2. Compilar o Código C++ para a Arquitetura ARM do Kit
```bash
# Opção A — Script automatizado:
bash cross_build.sh

# Opção B — Via Makefile:
make cross

# Opção C — Comando direto:
arm-buildroot-linux-gnueabihf-g++ -Wall -Wextra -std=c++17 -O2 \
    -I../../include ../../src/IrObstacleSensor.cpp ../../src/main.cpp -o app_sensor_arm
```

### 3. Transferir o Executável para o Kit via `scp`
Com o kit conectado à rede Ethernet (IP padrão da placa: `192.168.42.2`, IP do PC: `192.168.42.10`):
```bash
scp app_sensor_arm root@192.168.42.2:/home/root/
# (Senha padrão da placa: root)
```

### 4. Descobrir o Canal ADC e Executar no Kit (via SSH ou Serial TeraTerm 115200 8N1)
```bash
ssh root@192.168.42.2
cd /home/root
chmod +x app_sensor_arm

# Verificar resposta do canal IIO:
cat /sys/bus/iio/devices/iio:device0/in_voltage0_raw

# Executar o programa de monitoramento em tempo real:
./app_sensor_arm /sys/bus/iio/devices/iio:device0/in_voltage0_raw
```

### 5. Exemplo de Saída em Execução
```text
[0001] [SENSOR HW-201] Bruto: 850 (0.68 V) | Obstaculo: PRESENTE | Carga: INTEGRA (SEGURA)
[0002] [SENSOR HW-201] Bruto: 842 (0.68 V) | Obstaculo: PRESENTE | Carga: INTEGRA (SEGURA)
[0003] [SENSOR HW-201] Bruto: 3600 (2.90 V) | Obstaculo: AUSENTE | Carga: VIOLACAO DETECTADA!
```
